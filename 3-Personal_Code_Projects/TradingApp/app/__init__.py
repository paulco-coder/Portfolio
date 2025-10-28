from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple

import pandas as pd
from flask import Flask, jsonify, render_template, request  # type: ignore

from .backtest import (
    BACKTEST_TIMEFRAMES,
    BacktestConfig,
    BacktestError,
    PairsTradingBacktester,
    default_backtest_config,
)
from .pair_metrics import build_pair_table, clear_metrics_cache
from .price_memory import ensure_price_memory
from .zscore import ZScoreError, compute_pair_indicators_with_chart

_APP_DIR = Path(__file__).resolve().parent
_TOP_PAIRS_PATH = _APP_DIR / "top_pairs.csv"
_DATA_DIR = _APP_DIR / "data"
_TIMEFRAME_MINUTES: Dict[str, int] = {str(entry["id"]): int(entry.get("minutes", 5)) for entry in BACKTEST_TIMEFRAMES}


def _load_pairs_dataframe() -> pd.DataFrame:
    if not _TOP_PAIRS_PATH.exists():
        raise FileNotFoundError(f"CSV not found: {_TOP_PAIRS_PATH}")

    df = pd.read_csv(_TOP_PAIRS_PATH, usecols=[0, 1]).dropna(how="any") if _TOP_PAIRS_PATH.exists() else pd.DataFrame(columns=["a", "b"])
    if df.empty:
        return pd.DataFrame(columns=["a", "b"])

    df = df.copy()
    df["a"] = df["a"].astype(str).str.strip()
    df["b"] = df["b"].astype(str).str.strip()
    df = df[(df["a"] != "") & (df["b"] != "")]
    return df


def _write_pairs_dataframe(df: pd.DataFrame) -> None:
    sanitized = df.loc[:, ["a", "b"]].copy()
    sanitized["a"] = sanitized["a"].astype(str).str.strip().str.upper()
    sanitized["b"] = sanitized["b"].astype(str).str.strip().str.upper()
    sanitized.to_csv(_TOP_PAIRS_PATH, index=False)


def _pair_key(symbol_a: str, symbol_b: str) -> Tuple[str, str]:
    normalized_a = _normalize_token(symbol_a)
    normalized_b = _normalize_token(symbol_b)
    return tuple(sorted((normalized_a, normalized_b)))


def _active_symbols_from_pairs(pairs_df: pd.DataFrame) -> Set[str]:
    symbols: Set[str] = set()
    for raw_a, raw_b in pairs_df[["a", "b"]].itertuples(index=False, name=None):
        normalized_a = _normalize_token(str(raw_a))
        normalized_b = _normalize_token(str(raw_b))
        if normalized_a:
            symbols.add(normalized_a)
        if normalized_b:
            symbols.add(normalized_b)
    return symbols


def _cleanup_unused_symbol_files(active_symbols: Set[str]) -> None:
    if not _DATA_DIR.exists():
        return
    for file_path in _DATA_DIR.glob("*_5m.csv"):
        symbol = file_path.stem.split("_", 1)[0].upper()
        if symbol not in active_symbols:
            try:
                file_path.unlink()
            except OSError:
                continue


def _normalize_backtest_datetime(raw_value: Optional[str], timeframe_minutes: int) -> Optional[str]:
    if not raw_value:
        return None
    cleaned = str(raw_value).strip()
    if not cleaned:
        return None
    try:
        parsed = datetime.fromisoformat(cleaned)
    except ValueError:
        return None

    minutes = max(1, timeframe_minutes)
    total_minutes = parsed.hour * 60 + parsed.minute
    floored_total = (total_minutes // minutes) * minutes
    aligned_hour = floored_total // 60
    aligned_minute = floored_total % 60
    aligned = parsed.replace(hour=aligned_hour, minute=aligned_minute, second=0, microsecond=0)
    return aligned.strftime("%Y-%m-%dT%H:%M")


def create_app():
    app = Flask(__name__, template_folder="templates")

    @app.route("/")
    def home():
        return render_template("home.html")

    @app.route("/pair-trading")
    def pair_trading():
        pairs_df = _load_pairs_dataframe()
        clear_metrics_cache()

        symbols = {
            _normalize_token(value)
            for value in pairs_df.to_numpy().flatten()
            if isinstance(value, str) and value.strip()
        }

        for symbol in symbols:
            ensure_price_memory(symbol)

        table_rows = build_pair_table(pairs_df.to_dict("records"))

        return render_template("pair_trading.html", table_rows=table_rows)

    @app.route("/pair-trading/pairs/add", methods=["POST"])
    def pair_add():
        payload = request.get_json(silent=True) or request.form or {}

        raw_symbol_a = str(payload.get("symbol_a", "")).strip()
        raw_symbol_b = str(payload.get("symbol_b", "")).strip()
        symbol_a = _normalize_token(raw_symbol_a)
        symbol_b = _normalize_token(raw_symbol_b)

        if not symbol_a or not symbol_b:
            return jsonify({"error": "Symbols A and B are required"}), 400
        if symbol_a == symbol_b:
            return jsonify({"error": "Symbols must be different"}), 400

        try:
            pairs_df = _load_pairs_dataframe()
        except FileNotFoundError:
            pairs_df = pd.DataFrame(columns=["a", "b"])

        existing_keys = {_pair_key(row["a"], row["b"]) for _, row in pairs_df.iterrows()} if not pairs_df.empty else set()
        candidate_key = _pair_key(symbol_a, symbol_b)
        if candidate_key in existing_keys:
            return jsonify({"error": "Pair already exists"}), 409

        try:
            ensure_price_memory(symbol_a)
            ensure_price_memory(symbol_b)
        except Exception as exc:  # pragma: no cover - network errors
            return jsonify({"error": f"Failed to download data: {exc}"}), 502

        new_entry = pd.DataFrame([
            {
                "a": raw_symbol_a.upper() if raw_symbol_a else symbol_a,
                "b": raw_symbol_b.upper() if raw_symbol_b else symbol_b,
            }
        ])

        updated_df = pd.concat([pairs_df, new_entry], ignore_index=True) if not pairs_df.empty else new_entry
        _write_pairs_dataframe(updated_df)
        clear_metrics_cache()

        return jsonify({"status": "ok", "pair": {"a": updated_df.iloc[-1]["a"], "b": updated_df.iloc[-1]["b"]}}), 201

    @app.route("/pair-trading/pairs/remove", methods=["POST"])
    def pair_remove():
        payload = request.get_json(silent=True) or request.form or {}

        symbol_a = _normalize_token(str(payload.get("symbol_a", "")))
        symbol_b = _normalize_token(str(payload.get("symbol_b", "")))

        if not symbol_a or not symbol_b:
            return jsonify({"error": "Symbols A and B are required"}), 400

        try:
            pairs_df = _load_pairs_dataframe()
        except FileNotFoundError:
            return jsonify({"error": "No registered pair"}), 404

        if pairs_df.empty:
            return jsonify({"error": "No registered pair"}), 404

        normalized_a = pairs_df["a"].apply(_normalize_token)
        normalized_b = pairs_df["b"].apply(_normalize_token)
        mask = (normalized_a == symbol_a) & (normalized_b == symbol_b)

        if not mask.any():
            return jsonify({"error": "Pair not found"}), 404

        updated_df = pairs_df.loc[~mask, ["a", "b"]]
        _write_pairs_dataframe(updated_df)

        active_symbols = _active_symbols_from_pairs(updated_df)
        _cleanup_unused_symbol_files(active_symbols)
        clear_metrics_cache()

        return jsonify({"status": "ok", "remaining_pairs": len(updated_df)}), 200

    @app.route("/pair-trading/trade", methods=["GET", "POST"])
    def pair_backtest():
        symbol_a = _normalize_token(request.values.get("symbol_a", ""))
        symbol_b = _normalize_token(request.values.get("symbol_b", ""))

        if not symbol_a or not symbol_b:
            raise ValueError("Parameters symbol_a and symbol_b are required")

        ensure_price_memory(symbol_a)
        ensure_price_memory(symbol_b)

        backtest_result_payload: Optional[Dict[str, object]] = None
        backtest_error: Optional[str] = None

        if request.method == "POST":
            timeframe_value = request.form.get("timeframe", "").strip()
            window_raw = request.form.get("window", "")
            threshold_raw = request.form.get("threshold_multiplier", "")
            exit_threshold_raw = request.form.get("exit_threshold", "")
            start_raw = request.form.get("start_at", "")
            end_raw = request.form.get("end_at", "")
            fee_raw = request.form.get("fee_percent", "")
            capital_raw = request.form.get("initial_capital", "")
            volatility_flag_raw = request.form.get("use_volatility_adjustment")

            valid_timeframes = {entry["id"] for entry in BACKTEST_TIMEFRAMES}
            timeframe = timeframe_value if timeframe_value in valid_timeframes else "5min"
            timeframe_minutes = _TIMEFRAME_MINUTES.get(timeframe, 5)

            try:
                window_value = int(window_raw)
            except (TypeError, ValueError):
                window_value = 600
            window_value = max(3, min(window_value, 2000))

            try:
                threshold_value = float(threshold_raw)
            except (TypeError, ValueError):
                threshold_value = 2.0
            threshold_value = max(0.1, min(threshold_value, 10.0))

            try:
                exit_threshold_value = float(exit_threshold_raw)
            except (TypeError, ValueError):
                exit_threshold_value = 0.0
            exit_threshold_value = max(0.0, min(exit_threshold_value, 10.0))

            start_aligned = _normalize_backtest_datetime(start_raw, timeframe_minutes)
            end_aligned = _normalize_backtest_datetime(end_raw, timeframe_minutes)

            if start_aligned and end_aligned:
                start_dt = datetime.fromisoformat(start_aligned)
                end_dt = datetime.fromisoformat(end_aligned)
                if end_dt < start_dt:
                    end_aligned = start_aligned
            else:
                defaults = default_backtest_config(symbol_a, symbol_b)
                start_aligned = _normalize_backtest_datetime(defaults.start_at, timeframe_minutes)
                end_aligned = _normalize_backtest_datetime(defaults.end_at, timeframe_minutes)

            try:
                fee_value = float(fee_raw)
            except (TypeError, ValueError):
                fee_value = 0.1
            fee_value = max(0.0, min(fee_value, 10.0))

            try:
                initial_capital = float(capital_raw)
            except (TypeError, ValueError):
                initial_capital = 1000.0
            initial_capital = max(10.0, min(initial_capital, 1_000_000.0))

            use_volatility_adjustment = bool(volatility_flag_raw)

            config = BacktestConfig(
                symbol_a=symbol_a,
                symbol_b=symbol_b,
                timeframe=timeframe,
                window=window_value,
                threshold_multiplier=threshold_value,
                start_at=start_aligned,
                end_at=end_aligned,
                fee_percent=fee_value,
                initial_capital=initial_capital,
                use_volatility_adjustment=use_volatility_adjustment,
                exit_threshold=exit_threshold_value,
            )

            try:
                backtester = PairsTradingBacktester(config, _DATA_DIR)
                backtest_result_payload = backtester.run().to_dict()
            except BacktestError as exc:
                backtest_error = str(exc)
            except Exception as exc:  # pragma: no cover - defensive guard
                backtest_error = f"Unexpected error during backtest: {exc}"
        else:
            config = default_backtest_config(symbol_a, symbol_b)
        pair_label = f"{symbol_a} / {symbol_b}"

        return render_template(
            "pair_trade.html",
            pair_label=pair_label,
            symbol_a=symbol_a,
            symbol_b=symbol_b,
            timeframes=BACKTEST_TIMEFRAMES,
            config=config,
            is_post=request.method == "POST",
            backtest_result=backtest_result_payload,
            backtest_error=backtest_error,
        )

    @app.route("/pair-trading/indicators", methods=["POST"])
    def pair_indicators():
        payload = request.get_json(silent=True) or {}

        symbol_a = _normalize_token(str(payload.get("symbol_a", "")))
        symbol_b = _normalize_token(str(payload.get("symbol_b", "")))

        if not symbol_a or not symbol_b:
            return jsonify({"error": "Parameters symbol_a and symbol_b are required"}), 400

        try:
            window_raw = int(payload.get("window", 600))
        except (TypeError, ValueError):
            return jsonify({"error": "Invalid window parameter"}), 400

        try:
            threshold_raw = float(payload.get("threshold_multiplier", 2.0))
        except (TypeError, ValueError):
            return jsonify({"error": "Invalid threshold_multiplier parameter"}), 400

        window = max(3, min(window_raw, 2000))
        threshold_multiplier = max(0.1, min(threshold_raw, 10.0))

        ensure_price_memory(symbol_a)
        ensure_price_memory(symbol_b)

        try:
            indicators, charts = compute_pair_indicators_with_chart(
                symbol_a=symbol_a,
                symbol_b=symbol_b,
                window=window,
                threshold_multiplier=threshold_multiplier,
            )
        except ZScoreError as exc:
            return jsonify({"error": str(exc)}), 400

        return jsonify(
            {
                "pair_label": f"{symbol_a} / {symbol_b}",
                "window": window,
                "threshold_multiplier": threshold_multiplier,
                "indicators": [
                    {
                        "timeframe": indicator.timeframe,
                        "z_score": indicator.z_score,
                        "threshold": indicator.threshold,
                        "signal": indicator.signal,
                        "points_used": indicator.points_used,
                        "std_dev": indicator.std_dev,
                    }
                    for indicator in indicators
                ],
                "charts": charts or {},
            }
        )

    @app.route("/pair-trading/analysis")
    def pair_analysis():
        symbol_a = _normalize_token(request.args.get("symbol_a", ""))
        symbol_b = _normalize_token(request.args.get("symbol_b", ""))
        if not symbol_a or not symbol_b:
            raise ValueError("Parameters symbol_a and symbol_b are required")

        ensure_price_memory(symbol_a)
        ensure_price_memory(symbol_b)

        series_a = _load_price_series(symbol_a)
        series_b = _load_price_series(symbol_b)

        pair_label = f"{symbol_a} / {symbol_b}"

        return render_template(
            "pair_analysis.html",
            pair_label=pair_label,
            symbol_a=symbol_a,
            symbol_b=symbol_b,
            series_a_full=series_a["full"],
            series_a_recent=series_a["recent"],
            series_b_full=series_b["full"],
            series_b_recent=series_b["recent"],
        )

    return app


def _normalize_token(raw: str) -> str:
    token = raw.strip().upper()
    if "-" in token:
        token = token.split("-")[0]
    if "/" in token:
        token = token.split("/")[0]
    return token


def _load_price_series(symbol: str, limit: int = 2000) -> Dict[str, List[Dict[str, float | str]]]:
    path = _DATA_DIR / f"{symbol}_5m.csv"
    if not path.exists():
        return {"full": [], "recent": []}
    df = pd.read_csv(path)
    if df.empty or "timestamp" not in df.columns or "close" not in df.columns:
        return {"full": [], "recent": []}
    df = df.dropna(subset=["timestamp", "close"]).tail(limit)
    df["timestamp"] = pd.to_datetime(df["timestamp"], utc=True, errors="coerce")
    df = df.dropna(subset=["timestamp"])
    full_series = [
        {"time": ts.isoformat(), "price": float(price)}
        for ts, price in zip(df["timestamp"], df["close"])
    ]

    cutoff = pd.Timestamp.utcnow() - pd.Timedelta(hours=12)
    recent_df = df[df["timestamp"] >= cutoff]
    recent_series = [
        {"time": ts.isoformat(), "price": float(price)}
        for ts, price in zip(recent_df["timestamp"], recent_df["close"])
    ]

    return {"full": full_series, "recent": recent_series}


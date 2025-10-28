from __future__ import annotations

import json
from dataclasses import dataclass
from datetime import datetime, timedelta, timezone
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Set, Tuple

import numpy as np
import pandas as pd

try:  # statsmodels is optional; degrade gracefully if missing
    from statsmodels.tsa.stattools import coint
except Exception:  # pragma: no cover - optional dependency
    coint = None  # type: ignore[assignment]


_DATA_DIR = Path(__file__).resolve().parent / "data"
_METRICS_DIR = Path(__file__).resolve().parent / "data" / "metrics"
_TIMEFRAMES: Dict[str, timedelta] = {}


@dataclass(frozen=True)
class PairRow:
    symbol_a: str
    symbol_b: str
    pair_label: str
    correlation: str
    coint_score: str


def build_pair_table(
    pairs: Iterable[Dict[str, str]],
    target_pairs: Optional[Iterable[Tuple[str, str]]] = None,
) -> List[PairRow]:
    targets: Optional[Set[Tuple[str, str]]] = None
    if target_pairs is not None:
        targets = {(_normalize_symbol(a), _normalize_symbol(b)) for a, b in target_pairs}

    rows: List[PairRow] = []
    for pair in pairs:
        raw_a = str(pair.get("a", ""))
        raw_b = str(pair.get("b", ""))
        symbol_a = _normalize_symbol(raw_a)
        symbol_b = _normalize_symbol(raw_b)
        label = f"{symbol_a} / {symbol_b}"
        if targets is None or (symbol_a, symbol_b) in targets:
            metrics = _compute_metrics(symbol_a, symbol_b)
            _write_cached_metrics(symbol_a, symbol_b, metrics)
        else:
            metrics = _read_cached_metrics(symbol_a, symbol_b)
            if metrics is None:
                metrics = _blank_metrics()
                _write_cached_metrics(symbol_a, symbol_b, metrics)
        rows.append(
            PairRow(
                symbol_a=symbol_a,
                symbol_b=symbol_b,
                pair_label=label,
                correlation=_format_float(metrics.correlation),
                coint_score=_format_float(metrics.coint_score),
            )
        )
    return rows


@dataclass(frozen=True)
class _Metrics:
    correlation: Optional[float]
    coint_score: Optional[float]
    timeframe_returns: Dict[str, Optional[float]]


def _compute_metrics(symbol_a: str, symbol_b: str) -> _Metrics:
    df = _load_pair_data(symbol_a, symbol_b)
    if df is None or df.empty:
        return _blank_metrics()

    correlation = float(df["close_a"].corr(df["close_b"])) if len(df) > 2 else None

    coint_score: Optional[float] = None
    if coint is not None and len(df) > 50:
        try:
            _, p_value, _ = coint(df["close_a"], df["close_b"])
            coint_score = 1.0 - float(p_value)
        except Exception:
            coint_score = None

    returns: Dict[str, Optional[float]] = {}

    return _Metrics(correlation=correlation, coint_score=coint_score, timeframe_returns=returns)


def _blank_metrics() -> _Metrics:
    return _Metrics(
        correlation=None,
        coint_score=None,
    timeframe_returns={},
    )


def _load_pair_data(symbol_a: str, symbol_b: str) -> Optional[pd.DataFrame]:
    df_a = _load_symbol(symbol_a)
    df_b = _load_symbol(symbol_b)
    if df_a is None or df_b is None:
        return None
    merged = pd.merge(df_a, df_b, on="timestamp", how="inner", suffixes=("_a", "_b"))
    if merged.empty:
        return None
    merged = merged.sort_values("timestamp").reset_index(drop=True)
    return merged


def _load_symbol(symbol: str) -> Optional[pd.DataFrame]:
    path = _DATA_DIR / f"{symbol}_5m.csv"
    if not path.exists():
        return None
    df = pd.read_csv(path)
    if df.empty or "timestamp" not in df.columns or "close" not in df.columns:
        return None
    df = df.copy()
    df["timestamp"] = pd.to_datetime(df["timestamp"], utc=True, errors="coerce")
    df = df.dropna(subset=["timestamp", "close"])
    df = df.rename(columns={"close": f"close"})
    return df[["timestamp", "close"]]


def _normalize_symbol(token: str) -> str:
    token = token.strip().upper()
    if "-" in token:
        token = token.split("-", 1)[0]
    if "/" in token:
        token = token.split("/", 1)[0]
    return token


def _format_float(value: Optional[float]) -> str:
    if value is None or np.isnan(value):
        return "—"
    return f"{value:.3f}"


def _format_percent(value: Optional[float]) -> str:
    if value is None or np.isnan(value):
        return "—"
    return f"{value:+.2%}"


def _metrics_path(symbol_a: str, symbol_b: str) -> Path:
    key = f"{symbol_a}_{symbol_b}.json"
    return _METRICS_DIR / key


def _read_cached_metrics(symbol_a: str, symbol_b: str) -> Optional[_Metrics]:
    path = _metrics_path(symbol_a, symbol_b)
    if not path.exists():
        return None
    try:
        with path.open("r", encoding="utf-8") as fh:
            payload = json.load(fh)
    except (json.JSONDecodeError, OSError):
        return None
    if not isinstance(payload, dict):
        return None
    correlation = payload.get("correlation")
    coint_score = payload.get("coint_score")
    timeframe = payload.get("timeframe_returns", {})
    if not isinstance(timeframe, dict):
        timeframe = {}
    return _Metrics(
        correlation=float(correlation) if isinstance(correlation, (int, float)) else None,
        coint_score=float(coint_score) if isinstance(coint_score, (int, float)) else None,
        timeframe_returns={},
    )


def _write_cached_metrics(symbol_a: str, symbol_b: str, metrics: _Metrics) -> None:
    _METRICS_DIR.mkdir(parents=True, exist_ok=True)
    path = _metrics_path(symbol_a, symbol_b)
    payload = {
        "correlation": metrics.correlation,
        "coint_score": metrics.coint_score,
    "timeframe_returns": {},
    }
    with path.open("w", encoding="utf-8") as fh:
        json.dump(payload, fh, indent=2)


def clear_metrics_cache() -> None:
    if not _METRICS_DIR.exists():
        return
    for file in _METRICS_DIR.glob("*.json"):
        try:
            file.unlink()
        except OSError:
            continue
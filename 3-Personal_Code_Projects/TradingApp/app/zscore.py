from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

import numpy as np
import pandas as pd

_DATA_DIR = Path(__file__).resolve().parent / "data"

_TIMEFRAME_ORDER: Tuple[Tuple[str, str], ...] = (
    ("6h", "6h"),
    ("1h", "1h"),
    ("15min", "15min"),
    ("5min", "5min"),
)


@dataclass(frozen=True)
class ZScoreIndicator:
    timeframe: str
    z_score: Optional[float]
    threshold: float
    signal: str
    points_used: int
    std_dev: Optional[float]


class ZScoreError(RuntimeError):
    """Raised when Z-score computation cannot complete."""


def compute_pair_indicators(
    symbol_a: str,
    symbol_b: str,
    window: int,
    threshold_multiplier: float,
) -> List[ZScoreIndicator]:
    indicators, _ = _compute_indicators_and_chart(
        symbol_a=symbol_a,
        symbol_b=symbol_b,
        window=window,
        threshold_multiplier=threshold_multiplier,
        include_chart=False,
    )
    return indicators


def compute_pair_indicators_with_chart(
    symbol_a: str,
    symbol_b: str,
    window: int,
    threshold_multiplier: float,
) -> Tuple[List[ZScoreIndicator], Optional[Dict[str, Dict[str, Any]]]]:
    return _compute_indicators_and_chart(
        symbol_a=symbol_a,
        symbol_b=symbol_b,
        window=window,
        threshold_multiplier=threshold_multiplier,
        include_chart=True,
    )


def _compute_indicators_and_chart(
    *,
    symbol_a: str,
    symbol_b: str,
    window: int,
    threshold_multiplier: float,
    include_chart: bool,
) -> Tuple[List[ZScoreIndicator], Optional[Dict[str, Dict[str, Any]]]]:
    series_a = _load_price_series(symbol_a)
    series_b = _load_price_series(symbol_b)
    if series_a is None or series_b is None:
        raise ZScoreError("Missing price history for one or both symbols")

    frames = _build_timeframe_frames(
        series_a=series_a,
        series_b=series_b,
        window=window,
    )

    indicators = _compute_indicators_from_frames(
        frames=frames,
        threshold_multiplier=threshold_multiplier,
    )

    charts = (
        _build_charts_from_frames(
            frames=frames,
            threshold_multiplier=threshold_multiplier,
            window=window,
        )
        if include_chart
        else None
    )

    return indicators, charts


def _build_timeframe_frames(
    *,
    series_a: pd.Series,
    series_b: pd.Series,
    window: int,
) -> Dict[str, Optional[pd.DataFrame]]:
    frames: Dict[str, Optional[pd.DataFrame]] = {}
    min_points = max(window, 3)
    for label, freq in _TIMEFRAME_ORDER:
        frame = _prepare_timeframe_frame(series_a, series_b, freq)
        if frame is None or frame.empty:
            frames[label] = None
        else:
            frames[label] = frame.tail(min_points)
    return frames


def _compute_indicators_from_frames(
    *,
    frames: Dict[str, Optional[pd.DataFrame]],
    threshold_multiplier: float,
) -> List[ZScoreIndicator]:
    results: List[ZScoreIndicator] = []
    for label, _freq in _TIMEFRAME_ORDER:
        frame = frames.get(label)
        if frame is None or frame.empty:
            results.append(
                ZScoreIndicator(
                    timeframe=label,
                    z_score=None,
                    threshold=float(threshold_multiplier),
                    signal="neutral",
                    points_used=0,
                    std_dev=None,
                )
            )
            continue

        stats = _calculate_z_series_stats(frame)
        if stats is None:
            results.append(
                ZScoreIndicator(
                    timeframe=label,
                    z_score=None,
                    threshold=float(threshold_multiplier),
                    signal="neutral",
                    points_used=len(frame),
                    std_dev=None,
                )
            )
            continue

        z_series, _mean_spread, _std_spread, z_std = stats
        z_value = float(z_series.iloc[-1])
        threshold_value = float(threshold_multiplier * z_std) if z_std > 0 else float(threshold_multiplier)
        signal = _classify_signal(z_value, threshold_value)
        results.append(
            ZScoreIndicator(
                timeframe=label,
                z_score=z_value,
                threshold=threshold_value,
                signal=signal,
                points_used=len(frame),
                std_dev=z_std,
            )
        )

    return results


def _build_charts_from_frames(
    *,
    frames: Dict[str, Optional[pd.DataFrame]],
    threshold_multiplier: float,
    window: int,
) -> Dict[str, Dict[str, Any]]:
    charts: Dict[str, Dict[str, Any]] = {}
    for label, freq in _TIMEFRAME_ORDER:
        frame = frames.get(label)
        if frame is None or frame.empty:
            continue
        chart_data = _build_zscore_chart_for_frame(
            label=label,
            frame=frame,
            freq=freq,
            window=window,
            threshold_multiplier=threshold_multiplier,
        )
        if chart_data:
            charts[label] = chart_data
    return charts


def _build_zscore_chart_for_frame(
    *,
    label: str,
    frame: pd.DataFrame,
    freq: str,
    window: int,
    threshold_multiplier: float,
) -> Optional[Dict[str, Any]]:
    stats = _calculate_z_series_stats(frame)
    if stats is None:
        return None

    z_series, mean_spread, std_spread, z_std = stats
    points = [
        {"time": idx.isoformat(), "z_score": float(value)}
        for idx, value in z_series.items()
        if np.isfinite(value)
    ]
    if not points:
        return None

    latest_z = float(z_series.iloc[-1])
    z_values = [point["z_score"] for point in points]
    threshold_value = float(threshold_multiplier * z_std) if z_std > 0 else float(threshold_multiplier)
    try:
        interval_seconds = int(pd.to_timedelta(freq).total_seconds())
    except Exception:
        interval_seconds = 0

    return {
        "timeframe": label,
        "label": label,
        "points": points,
        "mean_spread": mean_spread,
        "std_spread": std_spread,
        "latest_z": latest_z,
        "threshold_multiplier": float(threshold_multiplier),
        "threshold_zscore": threshold_value,
        "points_used": len(frame),
        "z_std": z_std,
        "interval_seconds": interval_seconds,
    "window_requested": int(window),
        "min_z": float(min(z_values)),
        "max_z": float(max(z_values)),
        "range": {
            "start": points[0]["time"],
            "end": points[-1]["time"],
        },
    }


def _calculate_z_series_stats(
    df: pd.DataFrame,
) -> Optional[Tuple[pd.Series, float, float, float]]:
    if df.empty or "close_a" not in df.columns or "close_b" not in df.columns:
        return None

    values_a = df["close_a"].astype(float)
    values_b = df["close_b"].astype(float)

    if values_a.nunique() < 2 or values_b.nunique() < 2:
        return None

    norm_a = _minmax_normalize(values_a)
    norm_b = _minmax_normalize(values_b)
    if norm_a is None or norm_b is None:
        return None

    spread = (norm_a - norm_b).dropna()
    if spread.empty:
        return None

    mean_spread = float(spread.mean())
    std_spread = float(spread.std(ddof=0))
    if not np.isfinite(std_spread) or std_spread == 0:
        return None

    z_series = (spread - mean_spread) / std_spread
    z_series = z_series.dropna()
    if z_series.empty:
        return None

    z_std = float(z_series.std(ddof=0))
    if not np.isfinite(z_std) or z_std == 0:
        z_std = 1.0

    return z_series, mean_spread, std_spread, z_std


def _load_price_series(symbol: str) -> Optional[pd.Series]:
    path = _DATA_DIR / f"{symbol}_5m.csv"
    if not path.exists():
        return None
    try:
        df = pd.read_csv(path)
    except Exception:
        return None
    if df.empty or "timestamp" not in df.columns or "close" not in df.columns:
        return None

    series = df.loc[:, ["timestamp", "close"]].dropna()
    if series.empty:
        return None

    series["timestamp"] = pd.to_datetime(series["timestamp"], utc=True, errors="coerce")
    series = series.dropna(subset=["timestamp"])
    if series.empty:
        return None

    indexed = series.set_index("timestamp").sort_index()
    closes = indexed["close"].astype(float)
    return closes


def _prepare_timeframe_frame(
    series_a: pd.Series,
    series_b: pd.Series,
    freq: str,
) -> Optional[pd.DataFrame]:
    if series_a.empty or series_b.empty:
        return None

    if freq == "5min":
        merged = pd.concat(
            [series_a.rename("close_a"), series_b.rename("close_b")],
            axis=1,
            join="inner",
        )
    else:
        resampled_a = series_a.resample(freq).last().dropna()
        resampled_b = series_b.resample(freq).last().dropna()
        merged = pd.concat(
            [resampled_a.rename("close_a"), resampled_b.rename("close_b")],
            axis=1,
            join="inner",
        )

    merged = merged.dropna()
    if merged.empty:
        return None
    return merged


def _compute_window_zscore(df: pd.DataFrame) -> Optional[Tuple[float, float]]:
    stats = _calculate_z_series_stats(df)
    if stats is None:
        return None

    z_series, _mean_spread, _std_spread, z_std = stats
    z_value = float(z_series.iloc[-1])
    return z_value, z_std


def _minmax_normalize(values: pd.Series) -> Optional[pd.Series]:
    v_min = float(values.min())
    v_max = float(values.max())
    if v_max == v_min:
        return None
    return (values - v_min) / (v_max - v_min)


def _classify_signal(z_value: float, threshold: float) -> str:
    if z_value >= threshold:
        return "S1/L2"
    if z_value <= -threshold:
        return "L1/S2"
    return "neutral"

from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timedelta, timezone
from pathlib import Path
from typing import List, Tuple

import pandas as pd
import requests

_FETCH_INTERVAL = timedelta(minutes=5)
_HISTORY_DURATION = timedelta(days=60)
_MAX_BATCH_CANDLES = 1000
_BINANCE_BASE_URL = "https://api.binance.com/api/v3/klines"
_DATA_DIR = Path(__file__).resolve().parent / "data"


@dataclass(frozen=True)
class PriceSample:
    timestamp: datetime
    close: float


def ensure_price_memory(symbol: str) -> pd.DataFrame:
    """Ensure we have the last 60 days of 5-minute closes for *symbol*."""

    symbol = symbol.upper().strip()
    if not symbol:
        raise ValueError("Empty symbol")

    _DATA_DIR.mkdir(exist_ok=True)
    target_end = _floor_to_interval(datetime.now(timezone.utc))
    target_start = target_end - _HISTORY_DURATION

    store_path = _DATA_DIR / f"{symbol}_5m.csv"
    cached = _read_cache(store_path)
    if not cached.empty:
        cached = cached[cached["timestamp"] >= target_start]

    cached = cached.drop_duplicates(subset="timestamp").sort_values("timestamp")

    expected_index = pd.date_range(start=target_start, end=target_end, freq=_FETCH_INTERVAL, tz=timezone.utc)

    for _ in range(2):
        series = cached.set_index("timestamp").reindex(expected_index)
        missing_index = series.index[series["close"].isna()]
        if missing_index.empty:
            break
        missing_ranges = _missing_ranges(missing_index)
        any_new_data = False
        for range_start, range_end in missing_ranges:
            fetched = _fetch_candles(symbol, range_start, range_end)
            if fetched.empty:
                continue
            cached = pd.concat([cached, fetched], ignore_index=True)
            cached = cached.drop_duplicates(subset="timestamp").sort_values("timestamp")
            any_new_data = True
        if not any_new_data:
            break

    series = cached.set_index("timestamp").reindex(expected_index)
    if series.empty:
        raise RuntimeError(f"No data downloaded for {symbol}")

    missing_ratio = float(series["close"].isna().mean())
    if missing_ratio > 0.5:
        raise RuntimeError(f"More than 50% of the data is missing for {symbol}")

    if series["close"].isna().any():
        series["close"] = series["close"].interpolate(method="time").ffill().bfill()

    final_df = series.reset_index().rename(columns={"index": "timestamp"})
    _write_cache(store_path, final_df)
    return final_df


def _fetch_candles(symbol: str, start: datetime, end: datetime) -> pd.DataFrame:
    start = _align_to_interval(start)
    end = _align_to_interval(end)
    if start > end:
        return pd.DataFrame(columns=["timestamp", "close"])

    symbol_pair = f"{symbol}USDT"
    step_ms = int(_FETCH_INTERVAL.total_seconds() * 1000)
    start_ms = int(start.timestamp() * 1000)
    end_ms = int(end.timestamp() * 1000)

    samples: List[PriceSample] = []
    current_ms = start_ms

    while current_ms <= end_ms:
        params = {
            "symbol": symbol_pair,
            "interval": "5m",
            "startTime": current_ms,
            "limit": _MAX_BATCH_CANDLES,
        }
        try:
            response = requests.get(_BINANCE_BASE_URL, params=params, timeout=10)
            response.raise_for_status()
            payload = response.json()
        except requests.RequestException as exc:
            raise RuntimeError(f"Binance request failed for {symbol_pair}: {exc}") from exc
        if not isinstance(payload, list) or not payload:
            break

        for candle in payload:
            open_time_ms = int(candle[0])
            close_price = float(candle[4])
            if open_time_ms < start_ms or open_time_ms > end_ms:
                continue
            samples.append(PriceSample(timestamp=datetime.fromtimestamp(open_time_ms / 1000, tz=timezone.utc), close=close_price))

        last_open_ms = int(payload[-1][0])
        next_ms = last_open_ms + step_ms
        if next_ms <= current_ms:
            break
        current_ms = next_ms
        if len(payload) < _MAX_BATCH_CANDLES:
            break

    if not samples:
        return pd.DataFrame(columns=["timestamp", "close"])

    data = pd.DataFrame(samples)
    data = data.drop_duplicates(subset="timestamp").sort_values("timestamp")
    return data


def _read_cache(path: Path) -> pd.DataFrame:
    if not path.exists():
        return pd.DataFrame(columns=["timestamp", "close"])
    df = pd.read_csv(path)
    if df.empty:
        return df
    df["timestamp"] = pd.to_datetime(df["timestamp"], utc=True)
    df["close"] = pd.to_numeric(df["close"], errors="coerce")
    return df.dropna(subset=["timestamp", "close"])


def _write_cache(path: Path, df: pd.DataFrame) -> None:
    df = df.copy()
    df["timestamp"] = df["timestamp"].dt.tz_convert(timezone.utc)
    df.to_csv(path, index=False, date_format="%Y-%m-%dT%H:%M:%SZ")


def _align_to_interval(dt_obj: datetime) -> datetime:
    floored = _floor_to_interval(dt_obj)
    return floored


def _floor_to_interval(dt_obj: datetime) -> datetime:
    dt_obj = dt_obj.astimezone(timezone.utc)
    minute = dt_obj.minute - (dt_obj.minute % 5)
    return dt_obj.replace(minute=minute, second=0, microsecond=0)


def _missing_ranges(missing_index: pd.DatetimeIndex) -> List[Tuple[datetime, datetime]]:
    if missing_index.empty:
        return []
    ranges: List[Tuple[datetime, datetime]] = []
    start = missing_index[0]
    previous = start
    for current in missing_index[1:]:
        if (current - previous) > _FETCH_INTERVAL:
            ranges.append((_align_to_interval(start.to_pydatetime()), _align_to_interval(previous.to_pydatetime())))
            start = current
        previous = current
    ranges.append((_align_to_interval(start.to_pydatetime()), _align_to_interval(previous.to_pydatetime())))
    return ranges
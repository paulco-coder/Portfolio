from __future__ import annotations

from dataclasses import dataclass, field
from datetime import datetime, timedelta
from pathlib import Path
from typing import Dict, List, Optional, Tuple

import numpy as np
import pandas as pd
from typing import Union

# Timeframe options expressed as (identifier, human readable label, minutes span)
BACKTEST_TIMEFRAMES: List[Dict[str, str | int]] = [
    {"id": "5min", "label": "5 minutes", "minutes": 5},
    {"id": "15min", "label": "15 minutes", "minutes": 15},
    {"id": "1h", "label": "1 hour", "minutes": 60},
    {"id": "6h", "label": "6 hours", "minutes": 360},
]


@dataclass(frozen=True)
class BacktestConfig:
    symbol_a: str
    symbol_b: str
    timeframe: str
    window: int
    threshold_multiplier: float
    start_at: Optional[str]
    end_at: Optional[str]
    fee_percent: float
    initial_capital: float = 1000.0
    use_volatility_adjustment: bool = True
    volatility_window: int = 600
    exit_threshold: float = 0.0


def default_backtest_config(symbol_a: str, symbol_b: str) -> BacktestConfig:
    """Return a default configuration for a backtest session."""

    now_utc = datetime.utcnow()
    end_floor = _floor_datetime(now_utc, minutes=5)
    start_floor = _floor_datetime(end_floor - timedelta(days=60), minutes=5)

    return BacktestConfig(
        symbol_a=symbol_a,
        symbol_b=symbol_b,
        timeframe="15min",
        window=600,
        threshold_multiplier=2.0,
        start_at=start_floor.strftime("%Y-%m-%dT%H:%M"),
        end_at=end_floor.strftime("%Y-%m-%dT%H:%M"),
        fee_percent=0.1,
        initial_capital=1000.0,
        use_volatility_adjustment=True,
        volatility_window=600,
        exit_threshold=0.0,
    )


def _floor_datetime(dt_value: datetime, minutes: int) -> datetime:
    minutes = max(1, minutes)
    floored_minutes = (dt_value.minute // minutes) * minutes
    return dt_value.replace(minute=floored_minutes, second=0, microsecond=0)


TIMEFRAME_FREQ: Dict[str, str] = {
    "5min": "5T",
    "15min": "15T",
    "1h": "1H",
    "6h": "6H",
}


class BacktestError(RuntimeError):
    """Raised when the backtest cannot be executed."""


@dataclass(frozen=True)
class BacktestTrade:
    trade_type: str
    entry_time: datetime
    exit_time: Optional[datetime]
    entry_price_a: float
    entry_price_b: float
    size_a: float
    size_b: float
    exit_price_a: Optional[float]
    exit_price_b: Optional[float]
    pnl: float

    def to_dict(self) -> Dict[str, str | float]:
        return {
            "trade_type": self.trade_type,
            "entry_time": self.entry_time.isoformat(),
            "exit_time": self.exit_time.isoformat() if self.exit_time else "",
            "entry_price_a": round(self.entry_price_a, 6),
            "entry_price_b": round(self.entry_price_b, 6),
            "size_a": round(self.size_a, 2),
            "size_b": round(self.size_b, 2),
            "exit_price_a": round(self.exit_price_a, 6) if self.exit_price_a is not None else None,
            "exit_price_b": round(self.exit_price_b, 6) if self.exit_price_b is not None else None,
            "pnl": round(self.pnl, 4),
        }


@dataclass(frozen=True)
class BacktestSummary:
    initial_capital: float
    final_capital: float
    total_pnl: float
    return_pct: float
    annual_return_pct: float
    total_trades: int
    winning_trades: int
    losing_trades: int
    win_rate: float
    total_fees: float

    def to_dict(self) -> Dict[str, float | int]:
        return {
            "initial_capital": round(self.initial_capital, 2),
            "final_capital": round(self.final_capital, 2),
            "total_pnl": round(self.total_pnl, 2),
            "return_pct": round(self.return_pct, 2),
            "annual_return_pct": round(self.annual_return_pct, 2),
            "total_trades": self.total_trades,
            "winning_trades": self.winning_trades,
            "losing_trades": self.losing_trades,
            "win_rate": round(self.win_rate, 2),
            "total_fees": round(self.total_fees, 2),
        }


@dataclass(frozen=True)
class BacktestResult:
    summary: BacktestSummary
    trades: List[BacktestTrade]
    equity_curve: List[Tuple[datetime, float]]
    price_series: List[Tuple[datetime, float, float]] = field(default_factory=list)
    signals: List[Dict[str, object]] = field(default_factory=list)

    def to_dict(self) -> Dict[str, object]:
        price_series_payload = [
            {
                "time": stamp.isoformat(),
                "price_a": round(price_a, 6),
                "price_b": round(price_b, 6),
            }
            for stamp, price_a, price_b in self.price_series
        ]

        signals_payload: List[Dict[str, object]] = []
        for event in self.signals:
            event_time = event.get("time")
            if isinstance(event_time, datetime):
                time_str = event_time.isoformat()
            else:
                time_str = str(event_time)

            signals_payload.append(
                {
                    "time": time_str,
                    "kind": str(event.get("kind", "")),
                    "trade_type": str(event.get("trade_type", "")),
                    "price_a": round(float(event.get("price_a_norm", 0.0)), 6),
                    "price_b": round(float(event.get("price_b_norm", 0.0)), 6),
                }
            )

        return {
            "summary": self.summary.to_dict(),
            "trades": [trade.to_dict() for trade in self.trades],
            "equity_curve": [
                {"time": stamp.isoformat(), "capital": round(value, 2)}
                for stamp, value in self.equity_curve
            ],
            "price_series": price_series_payload,
            "signals": signals_payload,
        }


class PairsTradingBacktester:
    """Lightweight backtester inspired by the research notebook implementation."""

    def __init__(self, config: BacktestConfig, data_dir: Path):
        self.config = config
        self._data_dir = data_dir

    def run(self) -> BacktestResult:
        frame = self._prepare_price_frame()
        if frame.empty:
            raise BacktestError("Not enough data to execute the backtest")

        fee_rate = max(0.0, self.config.fee_percent) / 100.0
        capital = float(self.config.initial_capital)
        initial_capital = capital

        open_position: Optional[Dict[str, object]] = None
        equity_curve: List[Tuple[datetime, float]] = []
        trades: List[BacktestTrade] = []
        total_fees = 0.0

        min_a = float(frame["close_a"].min())
        range_a = float(frame["close_a"].max() - min_a)
        if range_a <= 0:
            range_a = 1.0
        frame["norm_close_a"] = (frame["close_a"] - min_a) / range_a

        min_b = float(frame["close_b"].min())
        range_b = float(frame["close_b"].max() - min_b)
        if range_b <= 0:
            range_b = 1.0
        frame["norm_close_b"] = (frame["close_b"] - min_b) / range_b

        price_series: List[Tuple[datetime, float, float]] = []
        signal_events: List[Dict[str, object]] = []

        exit_threshold_value = max(0.0, float(self.config.exit_threshold))
        exit_threshold_short_long = -exit_threshold_value
        exit_threshold_long_short = exit_threshold_value

        for timestamp, row in frame.iterrows():
            current_time = timestamp.to_pydatetime()
            price_a = float(row["close_a"])
            price_b = float(row["close_b"])
            z_score = float(row["z_score"])
            price_series.append(
                (
                    current_time,
                    float(row["norm_close_a"]),
                    float(row["norm_close_b"]),
                )
            )

            if open_position is not None:
                should_close = False
                if open_position["type"] == "short_long" and z_score <= exit_threshold_short_long:
                    should_close = True
                elif open_position["type"] == "long_short" and z_score >= exit_threshold_long_short:
                    should_close = True

                if should_close:
                    position_type = str(open_position["type"])
                    capital_change, close_fee, trade = self._close_position(
                        open_position,
                        current_time,
                        price_a,
                        price_b,
                        z_score,
                        fee_rate,
                    )
                    capital += capital_change
                    total_fees += close_fee
                    trades.append(trade)
                    signal_events.append(
                        {
                            "time": current_time,
                            "kind": "exit",
                            "trade_type": position_type,
                            "price_a_norm": float(row["norm_close_a"]),
                            "price_b_norm": float(row["norm_close_b"]),
                        }
                    )
                    open_position = None

            if open_position is None:
                weight_a = 0.5
                weight_b = 0.5
                if self.config.use_volatility_adjustment:
                    vol_a = float(row.get("vol_a", float("nan")))
                    vol_b = float(row.get("vol_b", float("nan")))
                    if np.isfinite(vol_a) and np.isfinite(vol_b) and vol_a > 0 and vol_b > 0:
                        weight_sum = vol_a + vol_b
                        weight_a = vol_b / weight_sum
                        weight_b = vol_a / weight_sum

                if z_score >= self.config.threshold_multiplier:
                    open_position = self._open_position(
                        "short_long",
                        current_time,
                        price_a,
                        price_b,
                        capital,
                        fee_rate,
                        weight_a,
                        weight_b,
                    )
                    fee_paid = float(open_position.get("open_fee", 0.0))
                    capital -= fee_paid
                    total_fees += fee_paid
                    signal_events.append(
                        {
                            "time": current_time,
                            "kind": "enter",
                            "trade_type": "short_long",
                            "price_a_norm": float(row["norm_close_a"]),
                            "price_b_norm": float(row["norm_close_b"]),
                        }
                    )
                elif z_score <= -self.config.threshold_multiplier:
                    open_position = self._open_position(
                        "long_short",
                        current_time,
                        price_a,
                        price_b,
                        capital,
                        fee_rate,
                        weight_a,
                        weight_b,
                    )
                    fee_paid = float(open_position.get("open_fee", 0.0))
                    capital -= fee_paid
                    total_fees += fee_paid
                    signal_events.append(
                        {
                            "time": current_time,
                            "kind": "enter",
                            "trade_type": "long_short",
                            "price_a_norm": float(row["norm_close_a"]),
                            "price_b_norm": float(row["norm_close_b"]),
                        }
                    )

            equity_curve.append((current_time, capital))

        if open_position is not None:
            last_time, last_prices = frame.index[-1].to_pydatetime(), frame.iloc[-1]
            position_type = str(open_position["type"])
            capital_change, close_fee, trade = self._close_position(
                open_position,
                last_time,
                float(last_prices["close_a"]),
                float(last_prices["close_b"]),
                float(last_prices["z_score"]),
                fee_rate,
            )
            capital += capital_change
            total_fees += close_fee
            trades.append(trade)
            equity_curve.append((last_time, capital))
            signal_events.append(
                {
                    "time": last_time,
                    "kind": "exit",
                    "trade_type": position_type,
                    "price_a_norm": float(last_prices["norm_close_a"]),
                    "price_b_norm": float(last_prices["norm_close_b"]),
                }
            )

        total_trades = len(trades)
        winning_trades = sum(1 for trade in trades if trade.pnl > 0)
        losing_trades = total_trades - winning_trades
        total_pnl = capital - initial_capital
        return_pct = (total_pnl / initial_capital) * 100 if initial_capital else 0.0
        duration_span = frame.index[-1] - frame.index[0]
        duration_days = duration_span.total_seconds() / 86400.0
        if duration_days <= 0:
            duration_days = 1.0
        annual_return_pct = return_pct * (365.0 / duration_days)
        win_rate = (winning_trades / total_trades) * 100 if total_trades else 0.0

        summary = BacktestSummary(
            initial_capital=initial_capital,
            final_capital=capital,
            total_pnl=total_pnl,
            return_pct=return_pct,
            annual_return_pct=annual_return_pct,
            total_trades=total_trades,
            winning_trades=winning_trades,
            losing_trades=losing_trades,
            win_rate=win_rate,
            total_fees=total_fees,
        )

        return BacktestResult(
            summary=summary,
            trades=trades,
            equity_curve=equity_curve,
            price_series=price_series,
            signals=signal_events,
        )

    def _open_position(
        self,
        trade_type: str,
        entry_time: datetime,
        price_a: float,
        price_b: float,
        capital: float,
        fee_rate: float,
        weight_a: float,
        weight_b: float,
    ) -> Dict[str, object]:
        weight_sum = weight_a + weight_b
        if weight_sum <= 0:
            weight_a = weight_b = 0.5
            weight_sum = 1.0

        normalized_weight_a = weight_a / weight_sum
        normalized_weight_b = weight_b / weight_sum

        allocation_a = capital * normalized_weight_a
        allocation_b = capital * normalized_weight_b
        notional = allocation_a + allocation_b
        open_fee = notional * fee_rate
        if trade_type == "short_long":
            size_a = -allocation_a
            size_b = allocation_b
        else:
            size_a = allocation_a
            size_b = -allocation_b
        return {
            "type": trade_type,
            "entry_time": entry_time,
            "entry_price_a": price_a,
            "entry_price_b": price_b,
            "allocation_a": allocation_a,
            "allocation_b": allocation_b,
            "weight_a": normalized_weight_a,
            "weight_b": normalized_weight_b,
            "size_a": size_a,
            "size_b": size_b,
            "open_fee": open_fee,
        }

    def _close_position(
        self,
        position: Dict[str, object],
        exit_time: datetime,
        price_a: float,
        price_b: float,
        z_score: float,
        fee_rate: float,
    ) -> Tuple[float, float, BacktestTrade]:
        allocation_a = float(position["allocation_a"])
        allocation_b = float(position["allocation_b"])
        entry_price_a = float(position["entry_price_a"])
        entry_price_b = float(position["entry_price_b"])
        size_a = float(position.get("size_a", 0.0))
        size_b = float(position.get("size_b", 0.0))
        notional = allocation_a + allocation_b
        close_fee = notional * fee_rate

        if position["type"] == "short_long":
            pnl_a = allocation_a * (entry_price_a - price_a) / entry_price_a
            pnl_b = allocation_b * (price_b - entry_price_b) / entry_price_b
        else:  # long_short
            pnl_a = allocation_a * (price_a - entry_price_a) / entry_price_a
            pnl_b = allocation_b * (entry_price_b - price_b) / entry_price_b

        pnl = pnl_a + pnl_b
        open_fee = float(position.get("open_fee", 0.0))
        trade_pnl = pnl - close_fee - open_fee
        net_change = pnl - close_fee

        trade = BacktestTrade(
            trade_type=str(position["type"]),
            entry_time=position["entry_time"],  # type: ignore[arg-type]
            exit_time=exit_time,
            entry_price_a=entry_price_a,
            entry_price_b=entry_price_b,
            size_a=size_a,
            size_b=size_b,
            exit_price_a=price_a,
            exit_price_b=price_b,
            pnl=trade_pnl,
        )

        return net_change, close_fee, trade

    def _prepare_price_frame(self) -> pd.DataFrame:
        freq = TIMEFRAME_FREQ.get(self.config.timeframe)
        if not freq:
            raise BacktestError(f"Unknown timeframe: {self.config.timeframe}")

        df_a = self._load_symbol_frame(self.config.symbol_a)
        df_b = self._load_symbol_frame(self.config.symbol_b)

        merged = pd.concat(
            [df_a.rename(columns={"close": "close_a"}), df_b.rename(columns={"close": "close_b"})],
            axis=1,
            join="inner",
        )

        if merged.empty:
            raise BacktestError("No overlapping data available for both symbols")

        merged = merged.resample(freq).last().dropna()

        start, end = self._resolve_range()
        merged = merged[(merged.index >= start) & (merged.index <= end)]

        if merged.empty:
            raise BacktestError("No data found in the requested time range")

        merged = merged[(merged["close_a"] > 0) & (merged["close_b"] > 0)]
        if merged.empty:
            raise BacktestError("Non-positive prices detected in the selected range")

        merged["ret_a"] = merged["close_a"].pct_change()
        merged["ret_b"] = merged["close_b"].pct_change()

        if self.config.use_volatility_adjustment:
            vol_window = max(2, int(self.config.volatility_window))
            merged["vol_a"] = merged["ret_a"].rolling(vol_window).std()
            merged["vol_b"] = merged["ret_b"].rolling(vol_window).std()

        spread = np.log(merged["close_a"]) - np.log(merged["close_b"])
        roll_mean = spread.rolling(self.config.window).mean()
        roll_std = spread.rolling(self.config.window).std()

        merged["z_score"] = (spread - roll_mean) / roll_std
        merged = merged.dropna(subset=["z_score"])

        if self.config.use_volatility_adjustment:
            merged = merged.dropna(subset=["vol_a", "vol_b"])

        if merged.shape[0] < max(self.config.window, 5):
            raise BacktestError("Window is too short to compute the z-score")

        return merged

    def _load_symbol_frame(self, symbol: str) -> pd.DataFrame:
        path = self._data_dir / f"{symbol}_5m.csv"
        if not path.exists():
            raise BacktestError(f"File not found for symbol {symbol}")

        df = pd.read_csv(path)
        if df.empty or "timestamp" not in df.columns or "close" not in df.columns:
            raise BacktestError(f"Invalid data for symbol {symbol}")

        df = df.copy()
        df["timestamp"] = pd.to_datetime(df["timestamp"], utc=True, errors="coerce")
        df["close"] = pd.to_numeric(df["close"], errors="coerce")
        df = df.dropna(subset=["timestamp", "close"])
        if df.empty:
            raise BacktestError(f"Insufficient data for symbol {symbol}")
        return df.set_index("timestamp").sort_index()

    def _resolve_range(self) -> Tuple[pd.Timestamp, pd.Timestamp]:
        minutes = TIMEFRAME_FREQ.get(self.config.timeframe)
        minutes_value = {
            "5T": 5,
            "15T": 15,
            "1H": 60,
            "6H": 360,
        }.get(minutes or "5T", 5)

        default_end = _floor_datetime(datetime.utcnow(), minutes_value)
        default_start = _floor_datetime(default_end - timedelta(days=60), minutes_value)

        start_raw = self.config.start_at or default_start.strftime("%Y-%m-%dT%H:%M")
        end_raw = self.config.end_at or default_end.strftime("%Y-%m-%dT%H:%M")

        try:
            start = pd.to_datetime(start_raw, utc=True)
        except (TypeError, ValueError):
            start = pd.to_datetime(default_start, utc=True)

        try:
            end = pd.to_datetime(end_raw, utc=True)
        except (TypeError, ValueError):
            end = pd.to_datetime(default_end, utc=True)

        if end < start:
            end = start

        return pd.Timestamp(start), pd.Timestamp(end)
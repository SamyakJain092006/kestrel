# 0003: One strategy interface, every runtime

Status: Accepted (2026-11-26)

## Context

Strategies must run unchanged in backtest (M5 venue), the generative simulator (M6)
and the live pipeline (M9). If each mode had its own API, tested code would differ
from run code.

## Decision

A strategy is a class with exactly three callbacks, called by a runtime on one thread:

| Callback        | Called when                                                        | Receives                                 |
| --------------- | ------------------------------------------------------------------ | ---------------------------------------- |
| on_market_event | a market event becomes visible (after feed latency)                | the event + a read-only view of the book |
| on_order_update | one of our orders is acked, rejected, filled (partly) or cancelled | the update record                        |
| on_timer        | a timer the strategy asked for fires                               | the current time                         |

- **The strategy never reaches outward.** It gets a context object from the runtime that
  provides: current time, place order, cancel order, set timer. No clocks, files,
  sockets or global state.
- **Client order ids** are assigned by the context, increasing from 1 each run.
- **Read-only book view.** The strategy can query best bid/ask, depth and its own orders,
  but cannot modify the book.
- **Every order goes through the risk gate (M8)** inside the context, in every runtime.
- **No allocation inside callbacks** once the run has started (enforced from M3 onwards).

## Alternatives considered

- Strategy pulls data in a loop: forces each runtime to fake a different loop. Rejected.
- Separate backtest and live APIs: guarantees drift between tested and real behaviour. Rejected.

## Consequences

- The same strategy binary logic runs in all three modes.
- Look-ahead is impossible by construction: the strategy only sees what the runtime delivers.
- Cost: runtimes must implement the context faithfully, including latency.

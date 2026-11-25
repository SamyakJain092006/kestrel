# 0002: Determinism by construction

Status: Accepted (2026-11-26)

## Context

Debugging, regression testing and trusting P&L differences all need repeatable runs.
Rule: same data + same seed + same config => byte-identical output.

## Decision

- **Simulated time only.** Inside the simulator, time comes from event timestamps
  (ITCH ns-since-midnight plus modelled latencies). `system_clock` / `steady_clock`
  are banned in simulation code. Real clocks are allowed only in benchmarks/telemetry.
- **One event queue, total order.** Pending events are ordered by (timestamp, sequence number).
  Every event gets a monotonically increasing sequence number when it is scheduled.
- **Tie rule.** At an equal timestamp, exchange (ITCH) events are processed before our own
  order arrivals. This is the pessimistic choice: we never jump ahead of real flow.
- **Randomness.** One `master_seed` in the run config. Each component (venue latency,
  queue model, strategy, ...) gets its own RNG seeded from (master*seed, component name),
  so adding a draw in one component never shifts another's stream.
  Engine: `std::mt19937_64` (its output is fixed by the C++ standard). We write our own
  distribution functions, because `std::uniform*\*\_distribution` etc. may give different
  results on different standard libraries (libstdc++ vs libc++).
- **No address-dependent behaviour.** Never order, hash or iterate by pointer value.
  Unordered containers may be used for lookup, but never iterated to make a decision
  or produce output.
- **Single-threaded simulation** per run. Parallelism only across independent runs.
- **Money in integers.** Cash and P&L are int64 in ITCH price units x shares.
  Floating point is allowed inside strategy maths, but `-ffast-math` is banned
  (it reorders arithmetic between builds).
- **Every byte defined.** Records are zero-initialised, including padding (see 0001).
- **Output hash.** Every output event (acks, fills, P&L marks) is fed into a 64-bit hash
  (FNV-1a to start). A test runs the same config twice and requires equal hashes;
  a saved "golden" hash catches unintended behaviour changes.

## Alternatives considered

- Multi-threaded simulation: faster, but scheduling order would leak into results. Rejected.
- One shared RNG: simpler, but any new draw changes every other component's numbers. Rejected.

## Consequences

- Any failure is reproducible from (data file, config, seed).
- Cost: discipline in code review; a determinism test must run in CI.

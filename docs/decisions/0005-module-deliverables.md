# 0005: Every module ships tests, a benchmark and a README section

Status: Accepted (2026-11-26)

## Context

A reviewer should be able to judge any single module alone, and every number in the
README must be reproducible with one command.

## Decision

Each module `<m>` is "done" only when it has:

1. **Code** in `src/<m>/` and `include/kestrel/<m>/`.
2. **Tests** in `tests/<m>/`, passing in Debug (with sanitizers) and Release.
3. **A benchmark** in `bench/<m>/`, run by `bench/run_all.sh`, writing raw results to
   `bench/results/<m>/`.
4. **A README section** with: what it does, why this design, the alternatives,
   a results table (p50/p99/p99.9/max, sample count, environment id), and limitations.

- `bench/run_all.sh` rebuilds first, sets and records the machine state
  (governor, turbo, power profile, pinning to CPU 2) in `bench/ENVIRONMENT.md`,
  runs every benchmark, then restores the machine state.
- Numbers in the README are copied only from `bench/results/`, never typed by hand.

## Consequences

- Slower to finish each module, but every claim can be checked.

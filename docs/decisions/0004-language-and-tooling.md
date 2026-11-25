# 0004: Language split and tooling

Status: Accepted (2026-11-26)

## Decision

- **C++20 for everything on the hot path** (decoder, book, queues, venue, risk, telemetry).
- **Python 3 for research and RL only**, calling C++ through pybind11. Python never sits
  on the hot path. Rust: not used.
- **Build:** CMake with presets: Debug (with ASan + UBSan), Release (-O3), RelWithDebInfo
  (-O2 -g, used for profiling). Ninja generator.
- **Compilers:** clang is primary; gcc must also build. Both use
  `-Wall -Wextra -Werror`. `-march=native` only in Release, and recorded in ENVIRONMENT.md.
- **Tests:** GoogleTest. Benchmarks: Google Benchmark for micro-benchmarks,
  HdrHistogram for latency distributions.
- **Style:** clang-format (LLVM base, 4-space indent) run before every commit.
  Namespace `kestrel::<module>`. Types PascalCase, functions and variables snake*case,
  private members end in `*`.
- **Thread sanitizer (TSan)** runs on all concurrency tests (M4 onwards).

## Consequences

- One toolchain, two compilers: catches compiler-specific bugs early.
- The legacy_book code keeps its old style; new code follows this record.

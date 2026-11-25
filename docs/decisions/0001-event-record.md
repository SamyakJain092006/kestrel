# 0001: One event record for all modules

Status: Accepted (2026-11-24)

## Context

Kestrel has many modules (decoder, book, venue, strategies, event store, logger).
If each used its own format, every pair would need a converter: more bugs, more latency.
So all modules read and write one fixed record type.

## Decision

- **Size:** 32 bytes, so two records fit in one 64-byte cache line.
- **Trivially copyable:** plain fields only (no pointers, strings or virtuals), so records
  can be copied byte-for-byte through queues and written to / read from mmap'd files.
- **Fields:**

| Field        | Type      | Bytes | Meaning                                |
| ------------ | --------- | ----- | -------------------------------------- |
| timestamp    | uint64    | 8     | ns since midnight (ITCH sends 6 bytes) |
| order_id     | uint64    | 8     | ITCH order reference number            |
| price        | int32     | 4     | ITCH units: 1/10000 dollar             |
| shares       | uint32    | 4     | quantity                               |
| stock_locate | uint16    | 2     | ITCH stock id                          |
| type         | uint8     | 1     | Add / Execute / Cancel / Delete / ...  |
| side         | uint8     | 1     | Buy / Sell                             |
| reserved     | 4 x uint8 | 4     | explicit padding, always zero          |

- **Prices are signed integers in ITCH units.** Binary floating point can't store most
  decimals exactly, so equal prices could compare unequal and create ghost levels.
  Signed, because price differences (spreads) can be negative.
- **Padding is explicit and zeroed.** Unwritten padding holds leftover junk, which would
  make identical events hash differently and break the determinism check (contract 2).
- **Replace (U) is split into Delete(old id) + Add(new id).** This matches the exchange:
  a replaced order loses its queue position, exactly like a delete plus a new add.
  It also keeps the record at 32 bytes (a single Replace record would need 36).
- **The decoder supplies the side for the split Add** using its own order-id -> side map,
  so every event is self-contained and any module can read it alone.
- **Versioning:** a version number and record size live once in each file's header, not
  in every record. Readers check them first and refuse mismatched files.
- Compile-time checks enforce the size (32) and trivial copyability.

## Alternatives considered

- **64 bytes:** room for future fields, but half the events per cache line, so twice the
  memory traffic when streaming a day. Rejected for now.
- **40 bytes (natural size with Replace's second id):** records would straddle cache lines.
  Rejected.
- **Book fills in the side for split Adds:** no duplicate state, but events would depend
  on each other and could not be read alone. Rejected.

## Consequences

- Good: dense, fast to stream, safe to copy, mmap and hash; one format everywhere.
- Cost: the decoder keeps an order-id -> side map for every live order
  (memory plus one hash lookup per Add/Delete/Replace).
- To verify later: measure that map's cost in M1; revisit 64 bytes if fields must grow.

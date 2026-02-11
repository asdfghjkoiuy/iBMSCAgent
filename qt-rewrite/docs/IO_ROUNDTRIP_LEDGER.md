# IO Roundtrip Ledger

Tracks parser/serializer parity for the same chart through `open -> save -> open`.

## Diff Scope
- Header fields (`#TITLE/#ARTIST/#BPM/...`)
- Resource tables (`#WAVxx/#BMPxx`)
- Numeric tables (`#BPMxx/#STOPxx/#SCROLLxx`)
- Note channels and value tokens
- Expansion block passthrough

## Cases
| Case ID | Input File | Focus | Result |
|---|---|---|---|
| `IO-001` | Pending sample | `#WAV/#BMP` survive roundtrip without loss | PENDING |
| `IO-002` | Pending sample | lane/channel mapping parity (`A*/D*/B*`) | PENDING |
| `IO-003` | Pending sample | LN serialization and paired-event reconstruction | PENDING |
| `IO-004` | Pending sample | expansion lines preserved (`#IF/#ENDIF/...`) | PENDING |

## Rule
- Case is `PASS` only if semantic diff is zero.
- Line-order-only differences are acceptable and should be documented explicitly.

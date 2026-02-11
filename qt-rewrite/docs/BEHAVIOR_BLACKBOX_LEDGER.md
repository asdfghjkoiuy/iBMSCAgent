# Behavior Blackbox Ledger

Records user-observed behavior parity checks independent from source-code inspection.

| Case ID | Scenario | Expected Behavior | Current Qt Result | Status |
|---|---|---|---|---|
| `BB-001` | Open chart with populated `#WAVxx` | WAV/OGG table auto-populates and rows playable | Under verification after channel/model rewrite | IN_PROGRESS |
| `BB-002` | Select note and preview audio | Immediate playback, no multi-second freeze | Under verification after audio/cache fixes | IN_PROGRESS |
| `BB-003` | Trackpad pinch on macOS | Pinch in = zoom out, pinch out = zoom in, smooth anchor zoom | Implemented path exists; requires final tuning pass | IN_PROGRESS |
| `BB-004` | Cmd + two-finger vertical scroll (macOS) | Viewport zoom at gesture anchor, not scrollbar range change | Implemented path exists; regression test pending | IN_PROGRESS |
| `BB-005` | Write mode on LN body | Existing LN selected/edited, no illegal stacked insert | Implemented hit rule update; needs validation | IN_PROGRESS |
| `BB-006` | Delete selected note on mac keyboard | `Delete` and `Backspace` both remove selected notes | Implemented | IN_PROGRESS |
| `BB-007` | Right-side WAV/BGI panel after restart | No collapse to tiny height | Section min-height floor added; needs restart check | IN_PROGRESS |

## Validation Protocol
1. Capture one screen recording per case after each major patch.
2. Mark case `DONE` only when behavior is stable across reopen/reload.
3. If case regresses, set `REGRESSION` and link commit hash in notes.

## Context

The iBMSCAgent rewrite has successfully established the barebones Qt/QML runtime infrastructure spanning standard C++ undo/redo mechanics and parsing the `.bms` structure. However, porting the VB.NET editor reveals an immediate necessity for hardcore logic components—specifically long note continuity modeling, multi-layered selection modifications (clipboard, time insertion), and mode separation for tool handling. The primary challenge is taking primitive data structures designed for single node ticks and transforming them to handle duration constraints while maintaining GPU rendering performance via the `QSGNode` Qt Scene Graph system. 

## Goals / Non-Goals

**Goals:**
- Design a cohesive node representation strategy for Long Notes (LNs) that minimizes friction within the `QUndoStack`.
- Define an agnostic, memory-efficient clipboard abstraction format for note payload duplication.
- Outline the algorithm to calculate displacement factors when performing time-scaling and measure-inserting.
- Implement an explicit Tool Mode state machine bridging C++ input handlers with QML UI elements.
- Plan dynamic sizing for columns via structural changes to `kPlayerChannels`.

**Non-Goals:**
- Completely rewriting the `model/BmsDocument.h` base struct from scratch.
- Full real-time waveform Fast Fourier transforms off external audio streams (just focus on statically visualizing what's parsed).
- Introducing multi-document concurrent editing instances just yet.

## Decisions

1. **Long Notes Representation:** 
   - *Decision:* Maintain the existing `Model::Note` structure, but utilize a specialized reserved variable (or a paired linking system via ID/value reference) to bridge the LN head and LN tail. The VB.NET iteration historically used the `length` property (or paired note pointers). We will favor expanding `Model::Note` to include an optional `durationInBeats` floating rational object.
   - *Rationale:* Instead of a linked list or secondary vector, using a duration inline reduces the complexity of rendering loops and parsing.

2. **State Machine for Editor Tools:**
   - *Decision:* Introduce an `enum class EditMode { Select, Write, Eraser }` injected as a property inside `ChartCanvasItem`. 
   - *Rationale:* By strictly partitioning the `mousePressEvent`, we mitigate overlapping rubber-band behaviors causing accidental note placements when dragging.

3. **Clipboard Management:**
   - *Decision:* Create a serialized format (e.g. JSON or simply a string of `Note` data equivalents) that `AppController` shoves into the OS System Clipboard via `QClipboard`.
   - *Rationale:* Moving data to the OS `QClipboard` allows the application natively to keep paste data intact. The command stack will process it strictly as a bulk `PlaceNotesCommand`.

4. **Time Expansion Operations:**
   - *Decision:* Build atomic logic in a `TimeSelectionCommand` extending `QUndoCommand`. It will accept ranges (startBeat to endBeat) and mutate every single note's measure/beat index falling in that timeframe.
   - *Rationale:* Since `EditorDocument` holds the array, keeping all mass-mutating actions confined inside the undo framework ensures we don't break sync.

## Risks / Trade-offs

- *Risk:* Transitioning `Note` structure to include `duration` might break backward compatibility with the existing parsing logic.
  - *Mitigation:* Ensure `durationInBeats` defaults to `0`. If parsing detects a `#LNOBJ`, the algorithm computes the duration backward from tail to head and assigns it to the head, deleting the tail entity completely.
- *Risk:* Heavy GPU allocation when painting the long note tail blocks inside `updatePaintNode`.
  - *Mitigation:* We will subclass from `QSGSimpleRectNode` and construct one unified contiguous rectangle for LNs regardless of screen bounds, letting Qt's internal culling handle invisible sectors.
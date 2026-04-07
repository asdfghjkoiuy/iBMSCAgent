#pragma once

namespace Model {

enum class ChannelType {
    BGM,            // channel 01: background music track
    BPM,            // channel 03: BPM change (hex value)
    BGA,            // channel 04: BGA (base layer)
    BGAExtended,    // channel 06, 07: extended BGA/poor BGA
    Stop,           // channel 09: STOP sequence
    NoteNormal,     // 11-19 (P1), 21-29 (P2): visible normal notes
    NoteLong,       // 51-59 (P1), 61-69 (P2): long notes
    NoteHidden,     // 31-39 (P1), 41-49 (P2): hidden notes (autoplay-style)
    NoteMine,       // D1-D9, E1-E9 area: landmine notes
    BpmExtended,    // channel 08: extended BPM table lookup
    Layer,          // channel 05: BGA layer
    Unknown
};

/// Returns the ChannelType for a given decimal channel index.
ChannelType classifyChannel(int ch);

/// Returns true if the channel carries player-visible notes (Normal or Long).
bool isPlayerNoteChannel(ChannelType t);

/// Returns true if the channel carries notes that should be rendered in the chart.
bool isVisibleChannel(ChannelType t);

} // namespace Model

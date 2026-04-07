#pragma once
#include <QString>

namespace Model {

/// Per-column configuration for the chart editor.
struct ColumnConfig {
    bool enabled = true;       ///< Whether this column is visible/active
    int width = 48;            ///< Pixel width of the column
    QString title;             ///< Display title (e.g. "A1", "BPM", "B1")
    QString bmsChannel;        ///< BMS channel string (e.g. "11", "16", "01")
    int identifier = 0;        ///< Internal channel identifier (decimal)
    bool isNoteColumn = true;  ///< Whether this column holds note data
    bool isNumeric = false;    ///< Whether values are numeric (BPM/STOP)
    bool isSound = true;       ///< Whether values reference WAV slots
};

} // namespace Model

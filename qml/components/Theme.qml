pragma Singleton
import QtQuick 2.15

QtObject {
    // ── Background layers (Ableton-inspired flat dark) ────────────────────────
    readonly property color bgBase:    "#0a0a0a"
    readonly property color bgSurface: "#1a1a1a"
    readonly property color bgPanel:   "#2a2a2a"
    readonly property color bgCard:    "#333333"
    readonly property color bgHover:   "#3d3d3d"

    // ── Text ──────────────────────────────────────────────────────────────────
    readonly property color textPrimary:   "#ffffff"
    readonly property color textSecondary: "#999999"
    readonly property color textDisabled:  "#555555"

    // ── Accent / highlight (warm orange) ──────────────────────────────────────
    readonly property color accent:      "#ff764d"
    readonly property color accentHover: "#ff8f6d"
    readonly property color accentMuted: "#994a30"

    // ── Status colors ─────────────────────────────────────────────────────────
    readonly property color success: "#7ec856"
    readonly property color warning: "#d4a843"
    readonly property color error:   "#d94452"

    // ── Borders ───────────────────────────────────────────────────────────────
    readonly property color border:      "#333333"
    readonly property color borderFaint: "#222222"

    // ── Geometry (flat — minimal rounding) ────────────────────────────────────
    readonly property int radiusSmall:  1
    readonly property int radiusMedium: 2
    readonly property int radiusLarge:  3

    readonly property int spacingXS: 4
    readonly property int spacingS:  8
    readonly property int spacingM:  12
    readonly property int spacingL:  16
    readonly property int spacingXL: 24

    // ── Typography ────────────────────────────────────────────────────────────
    readonly property int fontSizeSmall:  10
    readonly property int fontSizeNormal: 12
    readonly property int fontSizeLarge:  14
    readonly property int fontSizeTitle:  17

    readonly property string fontFamily: Qt.platform.os === "osx"
        ? "SF Pro Text" : (Qt.platform.os === "windows" ? "Segoe UI" : "Noto Sans")

    readonly property string monoFamily: Qt.platform.os === "osx"
        ? "Menlo" : (Qt.platform.os === "windows" ? "Consolas" : "Noto Sans Mono")

    // ── Section headers ───────────────────────────────────────────────────────
    readonly property int sectionFontSize: 10
    readonly property real sectionLetterSpacing: 1.5

    // ── Animation ─────────────────────────────────────────────────────────────
    readonly property int animFast:   120
    readonly property int animNormal: 180
    readonly property int animSlow:   300
}

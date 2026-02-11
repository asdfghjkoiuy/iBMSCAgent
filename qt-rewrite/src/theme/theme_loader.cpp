#include "theme/theme_loader.h"

#include <QDomDocument>
#include <QFile>
#include <QFileInfo>

#include <array>

namespace ibmsc {

namespace {
struct VbColumnDescriptor {
    int index = 0;
    int width = 40;
    const char* title = "";
    bool isNoteCol = false;
    bool isNumeric = false;
    bool isSound = false;
    bool isVisible = true;
    int identifier = 0;
    qint64 noteColor = 0;
    qint64 textColor = 0;
    qint64 longNoteColor = 0;
    qint64 longTextColor = 0;
    qint64 bgColor = 0;
};

const std::array<VbColumnDescriptor, 28> kVbColumns = {{
    {0, 50, "Measure", false, true, false, true, 0, 0, -16711681, 0, -16711681, 0},
    {1, 60, "SCROLL", true, true, false, true, 99, 0, -65536, 0, -65536, 0},
    {2, 60, "BPM", true, true, false, true, 3, 0, -65536, 0, -65536, 0},
    {3, 50, "STOP", true, true, false, true, 9, 0, -65536, 0, -65536, 0},
    {4, 5, "", false, false, false, true, 0, 0, 0, 0, 0, 0},
    {5, 42, "A1", true, false, true, true, 0x16, -5197648, -16777216, -4144960, -16777216, 352321535},
    {6, 30, "A2", true, false, true, true, 0x11, -10309377, -16777216, -9785097, -16777216, 335557631},
    {7, 42, "A3", true, false, true, true, 0x12, -5197648, -16777216, -4144960, -16777216, 352321535},
    {8, 45, "A4", true, false, true, true, 0x13, -14238, -16777216, -539030, -16777216, 385059596},
    {9, 42, "A5", true, false, true, true, 0x14, -5197648, -16777216, -4144960, -16777216, 352321535},
    {10, 30, "A6", true, false, true, true, 0x15, -10309377, -16777216, -9785097, -16777216, 335557631},
    {11, 42, "A7", true, false, true, true, 0x18, -5197648, -16777216, -4144960, -16777216, 352321535},
    {12, 0, "A8", true, false, true, true, 0x19, -8355712, -16777216, -7303024, -16777216, 0},
    {13, 5, "", false, false, false, true, 0, 0, 0, 0, 0, 0},
    {14, 42, "D1", true, false, true, false, 0x21, -5197648, -16777216, -4144960, -16777216, 352321535},
    {15, 30, "D2", true, false, true, false, 0x22, -10309377, -16777216, -9785097, -16777216, 335557631},
    {16, 42, "D3", true, false, true, false, 0x23, -5197648, -16777216, -4144960, -16777216, 352321535},
    {17, 45, "D4", true, false, true, false, 0x24, -14238, -16777216, -539030, -16777216, 385059596},
    {18, 42, "D5", true, false, true, false, 0x25, -5197648, -16777216, -4144960, -16777216, 352321535},
    {19, 30, "D6", true, false, true, false, 0x28, -10309377, -16777216, -9785097, -16777216, 335557631},
    {20, 42, "D7", true, false, true, false, 0x29, -5197648, -16777216, -4144960, -16777216, 352321535},
    {21, 40, "D8", true, false, true, false, 0x26, -8355712, -16777216, -7303024, -16777216, 0},
    {22, 5, "", false, false, false, false, 0, 0, 0, 0, 0, 0},
    {23, 40, "BGA", true, false, false, false, 4, -7546998, -16777216, -7285874, -16777216, 0},
    {24, 40, "LAYER", true, false, false, false, 7, -7546998, -16777216, -7285874, -16777216, 0},
    {25, 40, "POOR", true, false, false, false, 6, -7546998, -16777216, -7285874, -16777216, 0},
    {26, 5, "", false, false, false, false, 0, 0, 0, 0, 0, 0},
    {27, 40, "B", true, false, true, true, 1, -1998720, -16777216, -2325115, -16777216, 0},
}};

QColor vbColorToQColor(const QString& raw) {
    bool ok = false;
    const qint64 signedColor = raw.toLongLong(&ok);
    if (!ok) {
        return QColor(0, 0, 0);
    }
    const quint32 argb = static_cast<quint32>(signedColor);
    return QColor((argb >> 16) & 0xFF, (argb >> 8) & 0xFF, argb & 0xFF, (argb >> 24) & 0xFF);
}

QColor readValueColor(const QDomElement& parent, const QString& tag, const QColor& fallback) {
    const QDomElement n = parent.firstChildElement(tag);
    if (n.isNull()) {
        return fallback;
    }
    const QString v = n.attribute("Value");
    if (v.isEmpty()) {
        return fallback;
    }
    return vbColorToQColor(v);
}

bool readBoolAttribute(const QDomElement& e, const QString& attr, bool fallback) {
    if (!e.hasAttribute(attr)) {
        return fallback;
    }
    const QString v = e.attribute(attr).trimmed().toLower();
    if (v == "1" || v == "true" || v == "yes") {
        return true;
    }
    if (v == "0" || v == "false" || v == "no") {
        return false;
    }
    return fallback;
}

ThemeColumn fromDescriptor(const VbColumnDescriptor& d) {
    ThemeColumn c;
    c.index = d.index;
    c.width = d.width;
    c.title = QString::fromUtf8(d.title);
    c.identifier = d.identifier;
    c.isNoteCol = d.isNoteCol;
    c.isNumeric = d.isNumeric;
    c.isSound = d.isSound;
    c.isVisible = d.isVisible;
    c.noteColor = vbColorToQColor(QString::number(d.noteColor));
    c.textColor = vbColorToQColor(QString::number(d.textColor));
    c.longNoteColor = vbColorToQColor(QString::number(d.longNoteColor));
    c.longTextColor = vbColorToQColor(QString::number(d.longTextColor));
    c.bgColor = vbColorToQColor(QString::number(d.bgColor));
    return c;
}

ThemeColumn genericColumn(int index) {
    ThemeColumn c;
    c.index = index;
    c.width = 40;
    c.title = QString("C%1").arg(index);
    c.identifier = 1;
    c.isNoteCol = true;
    c.isNumeric = false;
    c.isSound = true;
    c.isVisible = true;
    c.noteColor = QColor(240, 200, 70);
    c.textColor = QColor(20, 20, 20);
    c.longNoteColor = QColor(240, 130, 70);
    c.longTextColor = QColor(20, 20, 20);
    c.bgColor = QColor(50, 55, 60, 120);
    return c;
}
} // namespace

Theme ThemeLoader::defaultTheme() {
    Theme t;
    t.name = "Default";
    t.columns.reserve(static_cast<int>(kVbColumns.size()));
    for (const auto& d : kVbColumns) {
        t.columns.push_back(fromDescriptor(d));
    }
    return t;
}

bool ThemeLoader::loadTheme(const QString& filePath, Theme& theme, QString* error) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        if (error) {
            *error = QString("Failed to open theme: %1").arg(f.errorString());
        }
        return false;
    }

    QDomDocument doc;
    QString parseErr;
    int errLine = 0;
    int errCol = 0;
    if (!doc.setContent(&f, &parseErr, &errLine, &errCol)) {
        if (error) {
            *error = QString("Theme XML parse error at %1:%2 (%3)").arg(errLine).arg(errCol).arg(parseErr);
        }
        return false;
    }

    Theme out = defaultTheme();
    out.sourcePath = filePath;
    out.name = QFileInfo(filePath).baseName();
    const QDomElement root = doc.documentElement();
    const QDomElement colsNode = root.firstChildElement("Columns");
    QDomElement col = colsNode.firstChildElement("Column");
    while (!col.isNull()) {
        const int idx = col.attribute("Index").toInt();
        if (idx < 0) {
            col = col.nextSiblingElement("Column");
            continue;
        }
        if (idx >= out.columns.size()) {
            out.columns.resize(idx + 1);
            for (int i = 0; i <= idx; ++i) {
                if (out.columns[i].index != i) {
                    out.columns[i] = genericColumn(i);
                }
            }
        }

        ThemeColumn c = out.columns[idx];
        c.index = idx;
        if (col.hasAttribute("Width")) c.width = col.attribute("Width").toInt();
        if (col.hasAttribute("Title")) c.title = col.attribute("Title");
        if (col.hasAttribute("Identifier")) c.identifier = col.attribute("Identifier").toInt();
        c.isNoteCol = readBoolAttribute(col, "isNoteCol", readBoolAttribute(col, "NoteCol", c.isNoteCol));
        c.isNumeric = readBoolAttribute(col, "isNumeric", readBoolAttribute(col, "Numeric", c.isNumeric));
        c.isSound = readBoolAttribute(col, "isSound", readBoolAttribute(col, "Sound", c.isSound));
        c.isVisible = readBoolAttribute(col, "isVisible", readBoolAttribute(col, "Visible", c.isVisible));
        if (col.hasAttribute("NoteColor")) c.noteColor = vbColorToQColor(col.attribute("NoteColor"));
        if (col.hasAttribute("TextColor")) c.textColor = vbColorToQColor(col.attribute("TextColor"));
        if (col.hasAttribute("LongNoteColor")) c.longNoteColor = vbColorToQColor(col.attribute("LongNoteColor"));
        if (col.hasAttribute("LongTextColor")) c.longTextColor = vbColorToQColor(col.attribute("LongTextColor"));
        if (col.hasAttribute("BG")) c.bgColor = vbColorToQColor(col.attribute("BG"));
        out.columns[idx] = c;
        col = col.nextSiblingElement("Column");
    }

    const QDomElement visual = root.firstChildElement("VisualSettings");
    if (!visual.isNull()) {
        out.visual.background = readValueColor(visual, "Bg", out.visual.background);
        out.visual.grid = readValueColor(visual, "Grid", out.visual.grid);
        out.visual.sub = readValueColor(visual, "Sub", out.visual.sub);
        out.visual.vLine = readValueColor(visual, "VLine", out.visual.vLine);
        out.visual.mLine = readValueColor(visual, "MLine", out.visual.mLine);
        out.visual.selected = readValueColor(visual, "kSelected", out.visual.selected);
        const QString opacity = visual.firstChildElement("kOpacity").attribute("Value");
        if (!opacity.isEmpty()) {
            out.visual.noteOpacity = opacity.toDouble();
        }
    }

    if (out.columns.isEmpty()) {
        out = defaultTheme();
        out.sourcePath = filePath;
        out.name = QFileInfo(filePath).baseName();
    }

    theme = out;
    return true;
}

} // namespace ibmsc

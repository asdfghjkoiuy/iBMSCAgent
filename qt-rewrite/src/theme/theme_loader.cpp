#include "theme/theme_loader.h"

#include <QDomDocument>
#include <QFile>
#include <QFileInfo>

namespace ibmsc {

namespace {
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
} // namespace

Theme ThemeLoader::defaultTheme() {
    Theme t;
    t.name = "Default";
    for (int i = 0; i < 28; ++i) {
        ThemeColumn c;
        c.index = i;
        c.width = (i % 2 == 0) ? 42 : 36;
        c.title = QString("C%1").arg(i);
        c.noteColor = QColor(240, 200, 70);
        c.textColor = QColor(20, 20, 20);
        c.longNoteColor = QColor(240, 130, 70);
        c.longTextColor = QColor(20, 20, 20);
        c.bgColor = QColor(50 + (i % 3) * 10, 55 + (i % 5) * 10, 60, 120);
        t.columns.push_back(c);
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
    out.columns.clear();

    const QDomElement root = doc.documentElement();
    const QDomElement colsNode = root.firstChildElement("Columns");
    QDomElement col = colsNode.firstChildElement("Column");
    while (!col.isNull()) {
        ThemeColumn c;
        c.index = col.attribute("Index").toInt();
        c.width = col.attribute("Width", "42").toInt();
        c.title = col.attribute("Title");
        c.noteColor = vbColorToQColor(col.attribute("NoteColor", "-1"));
        c.textColor = vbColorToQColor(col.attribute("TextColor", "-1"));
        c.longNoteColor = vbColorToQColor(col.attribute("LongNoteColor", "-1"));
        c.longTextColor = vbColorToQColor(col.attribute("LongTextColor", "-1"));
        c.bgColor = vbColorToQColor(col.attribute("BG", "0"));
        out.columns.push_back(c);
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

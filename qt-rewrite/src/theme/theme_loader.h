#pragma once

#include <QColor>
#include <QString>
#include <QVector>

namespace ibmsc {

struct ThemeColumn {
    int index = 0;
    int width = 40;
    QString title;
    QColor noteColor;
    QColor textColor;
    QColor longNoteColor;
    QColor longTextColor;
    QColor bgColor;
};

struct ThemeVisual {
    QColor background = QColor(0, 0, 0);
    QColor grid = QColor(90, 90, 90, 120);
    QColor sub = QColor(120, 120, 120, 80);
    QColor vLine = QColor(80, 80, 80, 180);
    QColor mLine = QColor(220, 220, 220, 170);
    QColor selected = QColor(255, 255, 0);
    double noteOpacity = 0.8;
};

struct Theme {
    QString name;
    QString sourcePath;
    QVector<ThemeColumn> columns;
    ThemeVisual visual;
};

class ThemeLoader {
public:
    static Theme defaultTheme();
    static bool loadTheme(const QString& filePath, Theme& theme, QString* error);
};

} // namespace ibmsc

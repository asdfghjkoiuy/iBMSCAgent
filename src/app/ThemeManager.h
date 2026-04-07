#pragma once
#include <QObject>
#include <QColor>
#include <QVariantMap>

namespace App {

/// Manages color themes. Exposes colors as QML-accessible properties.
class ThemeManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QColor background READ background NOTIFY themeChanged)
    Q_PROPERTY(QColor surface READ surface NOTIFY themeChanged)
    Q_PROPERTY(QColor accent READ accent NOTIFY themeChanged)
    Q_PROPERTY(QColor text READ text NOTIFY themeChanged)
    Q_PROPERTY(QColor dimText READ dimText NOTIFY themeChanged)
    Q_PROPERTY(QColor noteColor READ noteColor NOTIFY themeChanged)
    Q_PROPERTY(QColor noteLong READ noteLong NOTIFY themeChanged)
    Q_PROPERTY(QColor noteMine READ noteMine NOTIFY themeChanged)
    Q_PROPERTY(QColor beatLine READ beatLine NOTIFY themeChanged)
    Q_PROPERTY(QColor measureLine READ measureLine NOTIFY themeChanged)

public:
    explicit ThemeManager(QObject* parent = nullptr);

    QColor background() const { return m_colors.value("background", QColor(30,30,35)).value<QColor>(); }
    QColor surface() const { return m_colors.value("surface", QColor(40,42,54)).value<QColor>(); }
    QColor accent() const { return m_colors.value("accent", QColor(255,118,77)).value<QColor>(); }
    QColor text() const { return m_colors.value("text", QColor(255,255,255)).value<QColor>(); }
    QColor dimText() const { return m_colors.value("dimText", QColor(85,85,85)).value<QColor>(); }
    QColor noteColor() const { return m_colors.value("note", QColor(97,175,239)).value<QColor>(); }
    QColor noteLong() const { return m_colors.value("noteLong", QColor(152,195,121)).value<QColor>(); }
    QColor noteMine() const { return m_colors.value("noteMine", QColor(224,108,117)).value<QColor>(); }
    QColor beatLine() const { return m_colors.value("beatLine", QColor(60,62,70)).value<QColor>(); }
    QColor measureLine() const { return m_colors.value("measureLine", QColor(100,105,120)).value<QColor>(); }

    Q_INVOKABLE bool loadTheme(const QString& path);
    Q_INVOKABLE bool saveTheme(const QString& path);
    Q_INVOKABLE void resetToDefault();

signals:
    void themeChanged();

private:
    QVariantMap m_colors;
    void applyDefaults();
};

} // namespace App

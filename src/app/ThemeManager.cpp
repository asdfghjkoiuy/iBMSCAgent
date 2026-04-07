#include "app/ThemeManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace App {

ThemeManager::ThemeManager(QObject* parent) : QObject(parent) {
    applyDefaults();
}

void ThemeManager::applyDefaults() {
    m_colors["background"] = QColor(30, 30, 35);
    m_colors["surface"] = QColor(40, 42, 54);
    m_colors["accent"] = QColor(255, 118, 77);
    m_colors["text"] = QColor(255, 255, 255);
    m_colors["dimText"] = QColor(85, 85, 85);
    m_colors["note"] = QColor(97, 175, 239);
    m_colors["noteLong"] = QColor(152, 195, 121);
    m_colors["noteMine"] = QColor(224, 108, 117);
    m_colors["beatLine"] = QColor(60, 62, 70);
    m_colors["measureLine"] = QColor(100, 105, 120);
}

bool ThemeManager::loadTheme(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject()) return false;
    QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        m_colors[it.key()] = QColor(it.value().toString());
    }
    emit themeChanged();
    return true;
}

bool ThemeManager::saveTheme(const QString& path) {
    QJsonObject obj;
    for (auto it = m_colors.begin(); it != m_colors.end(); ++it) {
        obj[it.key()] = it.value().value<QColor>().name(QColor::HexArgb);
    }
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    return true;
}

void ThemeManager::resetToDefault() {
    applyDefaults();
    emit themeChanged();
}

} // namespace App

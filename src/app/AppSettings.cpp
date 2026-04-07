#include "app/AppSettings.h"
#include <QSettings>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace App {

static AppSettings* s_instance = nullptr;

AppSettings* AppSettings::instance() {
    if (!s_instance) s_instance = new AppSettings();
    return s_instance;
}

AppSettings::AppSettings(QObject* parent) : QObject(parent) {}

QStringList AppSettings::recentFiles() const {
    QSettings s;
    return s.value("recentFiles").toStringList();
}

void AppSettings::addRecentFile(const QString& path) {
    QSettings s;
    QStringList list = s.value("recentFiles").toStringList();
    list.removeAll(path);
    list.prepend(path);
    while (list.size() > 10) list.removeLast();
    s.setValue("recentFiles", list);
    emit recentFilesChanged();
}

void AppSettings::removeStaleRecentFiles() {
    QSettings s;
    QStringList list = s.value("recentFiles").toStringList();
    QStringList cleaned;
    for (const auto& f : list)
        if (QFileInfo::exists(f)) cleaned << f;
    if (cleaned != list) {
        s.setValue("recentFiles", cleaned);
        emit recentFilesChanged();
    }
}

int AppSettings::gridSubdivision() const {
    QSettings s;
    return s.value("gridSubdivision", 16).toInt();
}

void AppSettings::setGridSubdivision(int v) {
    QSettings s;
    s.setValue("gridSubdivision", v);
    emit gridSubdivisionChanged();
}

QString AppSettings::audioDevice() const {
    QSettings s;
    return s.value("audioDevice").toString();
}

void AppSettings::setAudioDevice(const QString& name) {
    QSettings s;
    s.setValue("audioDevice", name);
    emit audioDeviceChanged();
}

bool AppSettings::ntInputMode() const { QSettings s; return s.value("ntInputMode", true).toBool(); }
void AppSettings::setNtInputMode(bool v) { QSettings s; s.setValue("ntInputMode", v); emit ntInputModeChanged(); }

bool AppSettings::errorCheck() const { QSettings s; return s.value("errorCheck", false).toBool(); }
void AppSettings::setErrorCheck(bool v) { QSettings s; s.setValue("errorCheck", v); emit errorCheckChanged(); }

bool AppSettings::previewOnClick() const { QSettings s; return s.value("previewOnClick", false).toBool(); }
void AppSettings::setPreviewOnClick(bool v) { QSettings s; s.setValue("previewOnClick", v); emit previewOnClickChanged(); }

bool AppSettings::showFilename() const { QSettings s; return s.value("showFilename", false).toBool(); }
void AppSettings::setShowFilename(bool v) { QSettings s; s.setValue("showFilename", v); emit showFilenameChanged(); }

bool AppSettings::wavAutoIncrease() const { QSettings s; return s.value("wavAutoIncrease", false).toBool(); }
void AppSettings::setWavAutoIncrease(bool v) { QSettings s; s.setValue("wavAutoIncrease", v); emit wavAutoIncreaseChanged(); }

// --- Column Configs ---

QList<Model::ColumnConfig> AppSettings::defaultColumnConfigs() {
    // identifier = base-36 decoded channel number (e.g. "11" = 1*36+1 = 37)
    return {
        // Special columns
        {true,  50, "Measure", "00",  0,   false, true,  false},
        {true,  60, "BPM",     "03",  3,   true,  true,  false},
        {true,  50, "STOP",    "09",  9,   true,  true,  false},
        // P1 side (A-group) — matching VB iBMSC layout
        // BMS ch "16"=1*36+6=42, "11"=37, "12"=38, "13"=39, "14"=40, "15"=41, "18"=44, "19"=45
        {true,  42, "A1",      "16", 42,  true,  false, true},
        {true,  30, "A2",      "11", 37,  true,  false, true},
        {true,  42, "A3",      "12", 38,  true,  false, true},
        {true,  45, "A4",      "13", 39,  true,  false, true},
        {true,  42, "A5",      "14", 40,  true,  false, true},
        {true,  30, "A6",      "15", 41,  true,  false, true},
        {true,  42, "A7",      "18", 44,  true,  false, true},
        {false, 40, "A8",      "19", 45,  true,  false, true},
        // P2 side (D-group)
        // "21"=73, "22"=74, "23"=75, "24"=76, "25"=77, "28"=80, "29"=81, "26"=78
        {false, 42, "D1",      "21", 73,  true,  false, true},
        {false, 30, "D2",      "22", 74,  true,  false, true},
        {false, 42, "D3",      "23", 75,  true,  false, true},
        {false, 45, "D4",      "24", 76,  true,  false, true},
        {false, 42, "D5",      "25", 77,  true,  false, true},
        {false, 30, "D6",      "28", 80,  true,  false, true},
        {false, 42, "D7",      "29", 81,  true,  false, true},
        {false, 40, "D8",      "26", 78,  true,  false, true},
        // BGA: "04"=4, "07"=7, "06"=6
        {false, 40, "BGA",     "04",  4,  true,  false, false},
        {false, 40, "LAYER",   "07",  7,  true,  false, false},
        {false, 40, "POOR",    "06",  6,  true,  false, false},
        // BGM: "01"=1
        {true,  40, "B",       "01",  1,  true,  false, true},
    };
}

QList<Model::ColumnConfig> AppSettings::columnConfigs() const {
    return loadColumnConfigs();
}

void AppSettings::setColumnConfigs(const QList<Model::ColumnConfig>& configs) {
    saveColumnConfigs(configs);
    emit columnConfigsChanged();
}

QVariantList AppSettings::columnConfigsVariant() const {
    QVariantList result;
    for (const auto& c : loadColumnConfigs()) {
        QVariantMap m;
        m["enabled"] = c.enabled;
        m["width"] = c.width;
        m["title"] = c.title;
        m["bmsChannel"] = c.bmsChannel;
        m["identifier"] = c.identifier;
        m["isNoteColumn"] = c.isNoteColumn;
        m["isNumeric"] = c.isNumeric;
        m["isSound"] = c.isSound;
        result.append(m);
    }
    return result;
}

void AppSettings::setColumnEnabled(int index, bool enabled) {
    auto configs = loadColumnConfigs();
    if (index < 0 || index >= configs.size()) return;
    configs[index].enabled = enabled;
    saveColumnConfigs(configs);
    emit columnConfigsChanged();
}

void AppSettings::setColumnWidth(int index, int width) {
    auto configs = loadColumnConfigs();
    if (index < 0 || index >= configs.size()) return;
    configs[index].width = std::max(10, std::min(200, width));
    saveColumnConfigs(configs);
    emit columnConfigsChanged();
}

void AppSettings::resetColumnConfigs() {
    QSettings s;
    s.remove("columnConfigs");
    emit columnConfigsChanged();
}

void AppSettings::saveColumnConfigs(const QList<Model::ColumnConfig>& configs) {
    QJsonArray arr;
    for (const auto& c : configs) {
        QJsonObject obj;
        obj["enabled"] = c.enabled;
        obj["width"] = c.width;
        obj["title"] = c.title;
        obj["bmsChannel"] = c.bmsChannel;
        obj["identifier"] = c.identifier;
        obj["isNoteColumn"] = c.isNoteColumn;
        obj["isNumeric"] = c.isNumeric;
        obj["isSound"] = c.isSound;
        arr.append(obj);
    }
    QSettings s;
    s.setValue("columnConfigs", QJsonDocument(arr).toJson(QJsonDocument::Compact));
}

QList<Model::ColumnConfig> AppSettings::loadColumnConfigs() const {
    QSettings s;
    // Version check: reset if column config format changed
    int ver = s.value("columnConfigVersion", 0).toInt();
    if (ver < 2) {
        s.remove("columnConfigs");
        const_cast<QSettings&>(s).setValue("columnConfigVersion", 2);
        return defaultColumnConfigs();
    }

    QString json = s.value("columnConfigs").toString();
    if (json.isEmpty()) return defaultColumnConfigs();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isArray())
        return defaultColumnConfigs();

    QList<Model::ColumnConfig> result;
    for (const auto& v : doc.array()) {
        QJsonObject obj = v.toObject();
        Model::ColumnConfig c;
        c.enabled = obj["enabled"].toBool(true);
        c.width = obj["width"].toInt(48);
        c.title = obj["title"].toString();
        c.bmsChannel = obj["bmsChannel"].toString();
        c.identifier = obj["identifier"].toInt();
        c.isNoteColumn = obj["isNoteColumn"].toBool(true);
        c.isNumeric = obj["isNumeric"].toBool(false);
        c.isSound = obj["isSound"].toBool(true);
        result.append(c);
    }
    if (result.isEmpty()) return defaultColumnConfigs();

    // Safety: if all note columns are disabled, reset to defaults
    bool anyEnabled = false;
    for (const auto& c : result) {
        if (c.enabled && c.isNoteColumn) { anyEnabled = true; break; }
    }
    if (!anyEnabled) return defaultColumnConfigs();

    return result;
}

} // namespace App

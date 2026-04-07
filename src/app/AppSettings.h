#pragma once
#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QList>
#include "model/ColumnConfig.h"

namespace App {

class AppSettings : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList recentFiles READ recentFiles NOTIFY recentFilesChanged)
    Q_PROPERTY(int gridSubdivision READ gridSubdivision WRITE setGridSubdivision NOTIFY gridSubdivisionChanged)
    Q_PROPERTY(QString audioDevice READ audioDevice WRITE setAudioDevice NOTIFY audioDeviceChanged)
    Q_PROPERTY(QVariantList columnConfigs READ columnConfigsVariant NOTIFY columnConfigsChanged)
    Q_PROPERTY(bool ntInputMode READ ntInputMode WRITE setNtInputMode NOTIFY ntInputModeChanged)
    Q_PROPERTY(bool errorCheck READ errorCheck WRITE setErrorCheck NOTIFY errorCheckChanged)
    Q_PROPERTY(bool previewOnClick READ previewOnClick WRITE setPreviewOnClick NOTIFY previewOnClickChanged)
    Q_PROPERTY(bool showFilename READ showFilename WRITE setShowFilename NOTIFY showFilenameChanged)
    Q_PROPERTY(bool wavAutoIncrease READ wavAutoIncrease WRITE setWavAutoIncrease NOTIFY wavAutoIncreaseChanged)

public:
    static AppSettings* instance();

    QStringList recentFiles() const;
    void addRecentFile(const QString& path);
    void removeStaleRecentFiles();

    int gridSubdivision() const;
    void setGridSubdivision(int v);

    QString audioDevice() const;
    void setAudioDevice(const QString& name);

    bool ntInputMode() const;
    void setNtInputMode(bool v);
    bool errorCheck() const;
    void setErrorCheck(bool v);
    bool previewOnClick() const;
    void setPreviewOnClick(bool v);
    bool showFilename() const;
    void setShowFilename(bool v);
    bool wavAutoIncrease() const;
    void setWavAutoIncrease(bool v);

    QList<Model::ColumnConfig> columnConfigs() const;
    void setColumnConfigs(const QList<Model::ColumnConfig>& configs);
    QVariantList columnConfigsVariant() const;
    Q_INVOKABLE void setColumnEnabled(int index, bool enabled);
    Q_INVOKABLE void setColumnWidth(int index, int width);
    Q_INVOKABLE void resetColumnConfigs();

    static QList<Model::ColumnConfig> defaultColumnConfigs();

signals:
    void recentFilesChanged();
    void gridSubdivisionChanged();
    void audioDeviceChanged();
    void columnConfigsChanged();
    void ntInputModeChanged();
    void errorCheckChanged();
    void previewOnClickChanged();
    void showFilenameChanged();
    void wavAutoIncreaseChanged();

private:
    explicit AppSettings(QObject* parent = nullptr);
    void saveColumnConfigs(const QList<Model::ColumnConfig>& configs);
    QList<Model::ColumnConfig> loadColumnConfigs() const;
};

} // namespace App

#pragma once
#include <QObject>
#include <QString>
#include <memory>

namespace Audio { class AudioEngine; }
namespace Editor { class EditorDocument; }
namespace Player { class PlayerBridge; }

namespace App {

/// Top-level application controller registered as QML context property "app".
/// Orchestrates file open/save, playback, and wires C++ layers together.
class AppController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasDocument READ hasDocument NOTIFY hasDocumentChanged)

public:
    explicit AppController(Editor::EditorDocument* doc,
                           Audio::AudioEngine* audio,
                           QObject* parent = nullptr);
    ~AppController() override;

    bool hasDocument() const;

public slots:
    Q_INVOKABLE void newFile();
    /// Open a file directly by path (called from QML after FileDialog returns).
    Q_INVOKABLE void openFile(const QString& path);
    /// Request QML to show an open-file dialog. QML calls openFile(path) on accept.
    Q_INVOKABLE void requestOpenDialog();
    Q_INVOKABLE bool saveFile();
    /// Request QML to show a save-as dialog. QML calls saveFileTo(path) on accept.
    Q_INVOKABLE void requestSaveAsDialog();
    Q_INVOKABLE bool saveFileTo(const QString& path);
    Q_INVOKABLE void playFromPosition(double startTimeSec = 0.0);
    Q_INVOKABLE void stopPlayback();
    Q_INVOKABLE void closeFile();
    Q_INVOKABLE bool convertWavSlotToOgg(int slotIndex);
    Q_INVOKABLE bool importAudioToBgmLane(const QString& localFilePath, int channelIndex, double absoluteBeat);
    Q_INVOKABLE void constBpm(int targetBpm);
    Q_INVOKABLE void scaleBpm(double ratio);
    Q_INVOKABLE void launchPreview(int startMeasure = 0);
    Q_INVOKABLE void closePreview();

signals:
    void hasDocumentChanged();
    void errorOccurred(const QString& message);
    void openDialogRequested();
    void saveAsDialogRequested(const QString& currentPath);

private:
    void doOpen(const QString& path);
    bool doSave(const QString& path);

    Editor::EditorDocument* m_doc;
    Audio::AudioEngine* m_audio;
    std::unique_ptr<Player::PlayerBridge> m_player;
    bool m_hasActiveDocument = false;
};

} // namespace App

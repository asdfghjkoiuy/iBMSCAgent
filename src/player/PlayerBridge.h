#pragma once
#include "player/PlayerWindow.h"
#include "model/BmsDocument.h"
#include "audio/AudioEngine.h"
#include <QObject>
#include <QTimer>
#include <memory>

namespace Player {

class PlayerBridge : public QObject {
    Q_OBJECT
public:
    explicit PlayerBridge(QObject* parent = nullptr);
    ~PlayerBridge() override;

    void launchPreview(const Model::BmsDocument& doc,
                       Audio::AudioEngine* audioEngine,
                       int startMeasure = 0,
                       double scrollSpeed = 2.0);
    void closePreview();
    bool isRunning() const;

private slots:
    void onTick();

private:
    std::unique_ptr<PlayerWindow> m_window;
    QTimer m_loopTimer;
};

} // namespace Player

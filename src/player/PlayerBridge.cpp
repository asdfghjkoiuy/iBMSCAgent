#include "player/PlayerBridge.h"
#include "player/PlayerChart.h"

namespace Player {

PlayerBridge::PlayerBridge(QObject* parent) : QObject(parent) {
    connect(&m_loopTimer, &QTimer::timeout, this, &PlayerBridge::onTick);
    m_loopTimer.setInterval(0); // fire as fast as Qt event loop allows
}

PlayerBridge::~PlayerBridge() { closePreview(); }

void PlayerBridge::launchPreview(const Model::BmsDocument& doc,
                                 Audio::AudioEngine* audioEngine,
                                 int startMeasure,
                                 double scrollSpeed) {
    closePreview();
    auto chart = PlayerChart::build(doc, startMeasure);
    const auto* buffers = audioEngine ? audioEngine->soundBuffers() : nullptr;
    m_window = std::make_unique<PlayerWindow>();
    m_window->launch(std::move(chart), buffers, scrollSpeed);
    if (m_window->isRunning())
        m_loopTimer.start();
}

void PlayerBridge::closePreview() {
    m_loopTimer.stop();
    if (m_window) { m_window->requestClose(); m_window.reset(); }
}

bool PlayerBridge::isRunning() const {
    return m_window && m_window->isRunning();
}

void PlayerBridge::onTick() {
    if (!m_window || !m_window->isRunning()) {
        m_loopTimer.stop();
        return;
    }
    m_window->runLoop();
}

} // namespace Player

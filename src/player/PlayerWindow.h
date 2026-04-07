#pragma once
#include "player/PlayerChart.h"
#include "player/PlayerAudio.h"
#include "player/PlayerRenderer.h"
#include "audio/AudioEngine.h"
#include <QObject>
#include <array>
#include <memory>
#include <SDL2/SDL.h>

namespace Player {

static constexpr int kMaxLanes = kRendererMaxLanes;

// Pulsus default 7K+TT key bindings
static constexpr SDL_Keycode kLaneKeys[kMaxLanes] = {
    SDLK_LSHIFT, SDLK_s, SDLK_d, SDLK_f, SDLK_SPACE, SDLK_j, SDLK_k, SDLK_l,
};

class PlayerWindow : public QObject {
    Q_OBJECT
public:
    explicit PlayerWindow(QObject* parent = nullptr);
    ~PlayerWindow() override;

    void launch(PlayerChart chart, const std::vector<Audio::SoundBuffer>* buffers,
                double scrollSpeed = 2.0);
    void requestClose();
    bool isRunning() const { return m_running; }

    // Called by AppController each frame via a 0ms QTimer — runs the tight loop
    // for one iteration and returns immediately.
    Q_INVOKABLE void runLoop();

private:
    PlayerChart m_chart;
    std::unique_ptr<PlayerAudio> m_audio;
    std::unique_ptr<PlayerRenderer> m_renderer;
    double m_scrollSpeed = 2.0;
    double m_scrollTime  = 0.8;
    bool m_running = false;

    std::array<bool,   kMaxLanes> m_keyDown{};
    std::array<double, kMaxLanes> m_keyFade{};
    std::array<double, kMaxLanes> m_hitEffect{};
    double m_ttRotation = 0.0;
    double m_lastPos    = 0.0;

    Uint64 m_lastTick   = 0;
    Uint64 m_fpsCounter = 0;
    Uint64 m_fpsLast    = 0;
    int    m_fps        = 0;
};

} // namespace Player

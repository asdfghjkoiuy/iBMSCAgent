#include "player/PlayerWindow.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace Player {

static constexpr double kTargetFPS = 120.0;
static constexpr double kFrameTime = 1.0 / kTargetFPS;

PlayerWindow::PlayerWindow(QObject* parent) : QObject(parent) {
    m_keyDown.fill(false);
    m_keyFade.fill(0.0);
    m_hitEffect.fill(0.0);
}

PlayerWindow::~PlayerWindow() { requestClose(); }

void PlayerWindow::launch(PlayerChart chart,
                          const std::vector<Audio::SoundBuffer>* buffers,
                          double scrollSpeed) {
    if (m_running) return;

    SDL_SetHint(SDL_HINT_MAC_BACKGROUND_APP, "1");
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) return;

    m_renderer = std::make_unique<PlayerRenderer>();
    if (!m_renderer->init(1280, 720)) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        m_renderer.reset();
        return;
    }

    m_audio = std::make_unique<PlayerAudio>();
    if (!m_audio->init(buffers)) {
        m_renderer->shutdown();
        m_renderer.reset();
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return;
    }

    m_chart      = std::move(chart);
    m_scrollSpeed = scrollSpeed;
    m_scrollTime  = 1.6 / std::max(0.1, m_scrollSpeed);
    m_keyDown.fill(false);
    m_keyFade.fill(0.0);
    m_hitEffect.fill(0.0);
    m_ttRotation = 0.0;
    m_lastPos    = 0.0;
    m_lastTick   = SDL_GetPerformanceCounter();
    m_fpsLast    = m_lastTick;
    m_fpsCounter = 0;
    m_fps        = 0;

    m_audio->loadEvents(m_chart.events);
    m_audio->play();
    m_running = true;
}

void PlayerWindow::requestClose() {
    if (!m_running) return;
    m_running = false;
    if (m_audio)    { m_audio->stop(); m_audio->shutdown(); m_audio.reset(); }
    if (m_renderer) { m_renderer->shutdown(); m_renderer.reset(); }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void PlayerWindow::runLoop() {
    if (!m_running) return;

    // ── Frame limiter ─────────────────────────────────────────────────────────
    Uint64 now = SDL_GetPerformanceCounter();
    double dt  = static_cast<double>(now - m_lastTick) / SDL_GetPerformanceFrequency();
    if (dt < kFrameTime) return;
    m_lastTick = now;
    dt = std::min(dt, 0.05);

    // ── FPS counter ───────────────────────────────────────────────────────────
    ++m_fpsCounter;
    double fpsElapsed = static_cast<double>(now - m_fpsLast) / SDL_GetPerformanceFrequency();
    if (fpsElapsed >= 1.0) {
        m_fps = static_cast<int>(m_fpsCounter / fpsElapsed);
        m_fpsCounter = 0;
        m_fpsLast = now;
    }

    // ── SDL events ────────────────────────────────────────────────────────────
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT ||
            (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)) {
            requestClose();
            return;
        }
        if (ev.type == SDL_KEYDOWN && !ev.key.repeat) {
            if (ev.key.keysym.sym == SDLK_UP) {
                m_scrollSpeed = std::min(9.9, m_scrollSpeed + 0.5);
                m_scrollTime  = 1.6 / m_scrollSpeed;
            } else if (ev.key.keysym.sym == SDLK_DOWN) {
                m_scrollSpeed = std::max(0.5, m_scrollSpeed - 0.5);
                m_scrollTime  = 1.6 / m_scrollSpeed;
            }
            for (int i = 0; i < kMaxLanes; ++i) {
                if (ev.key.keysym.sym == kLaneKeys[i]) {
                    m_keyDown[i]   = true;
                    m_keyFade[i]   = 1.0;
                    m_hitEffect[i] = 10.0 / 120.0;
                }
            }
        }
        if (ev.type == SDL_KEYUP) {
            for (int i = 0; i < kMaxLanes; ++i)
                if (ev.key.keysym.sym == kLaneKeys[i])
                    m_keyDown[i] = false;
        }
    }

    // ── Update state ──────────────────────────────────────────────────────────
    for (int i = 0; i < kMaxLanes; ++i) {
        m_keyFade[i] = m_keyDown[i] ? 1.0 : std::max(0.0, m_keyFade[i] - dt / 0.12);
        if (m_hitEffect[i] > 0.0)
            m_hitEffect[i] = std::max(0.0, m_hitEffect[i] - dt);
    }

    // Turntable 45 RPM
    double rotSpeed = 45.0 / 60.0 * 2.0 * M_PI;
    m_ttRotation += (m_keyDown[0] ? -1.0 : 1.0) * rotSpeed * dt;
    m_ttRotation = std::fmod(m_ttRotation + 4.0 * M_PI, 2.0 * M_PI);

    // ── Auto hit effects ──────────────────────────────────────────────────────
    double pos = m_audio->position();
    for (const auto& e : m_chart.events) {
        if (e.lane < 0 || e.lane >= kMaxLanes) continue;
        if (e.timeSec > m_lastPos && e.timeSec <= pos) {
            m_hitEffect[e.lane] = 10.0 / 120.0;
            m_keyFade[e.lane]   = 1.0;
        }
    }
    m_lastPos = pos;

    // ── Render ────────────────────────────────────────────────────────────────
    m_renderer->render(m_chart, pos, m_scrollTime, m_scrollSpeed,
                       m_keyFade, m_hitEffect, m_ttRotation, m_fps);

    if (pos > m_chart.totalDuration + 1.0)
        requestClose();
}

} // namespace Player

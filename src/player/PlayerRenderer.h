#pragma once
#include "player/PlayerChart.h"
#include <SDL2/SDL.h>
#include <array>

namespace Player {

static constexpr int kRendererMaxLanes = 8;

class PlayerRenderer {
public:
    PlayerRenderer();
    ~PlayerRenderer();

    bool init(int width, int height);
    void shutdown();

    void render(const PlayerChart& chart, double currentTime,
                double scrollTime, double scrollSpeed,
                const std::array<double, kRendererMaxLanes>& keyFade,
                const std::array<double, kRendererMaxLanes>& hitEffect,
                double ttRotation, int fps);

    SDL_Window* window() const { return m_window; }

private:
    SDL_Window*   m_window   = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    int m_width  = 800;
    int m_height = 600;
};

} // namespace Player

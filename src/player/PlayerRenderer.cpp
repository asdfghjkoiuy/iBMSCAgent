#include "player/PlayerRenderer.h"
#include <cmath>
#include <algorithm>

namespace Player {

// ── Pulsus-exact layout (1280x720 window) ────────────────────────────────────
static const int kWinW        = 1280;
static const int kWinH        = 720;
static const int kLaneStartX  = 28;
static const int kLaneStartY  = 80;
static const int kNoteH       = 8;
static const int kLaneSpacing = 2;
// Judge line at bottom of lane area
// Pulsus: laneHeight from textureLaneBG1.height — we use 560px
static const int kLaneH       = 560;
static const int kJudgeY      = kLaneStartY + kLaneH; // 640
static const int kKeyAreaY    = kJudgeY + kNoteH + 4;
static const int kTTRadius    = 27;
static const int kBgaX        = 560;
static const int kBgaW        = 720;

// Lane widths: lane0=turntable(54), odd=white(34), even=blue(30)
static int laneW(int i) {
    if (i == 0) return 54;
    return (i % 2 == 1) ? 34 : 30;
}
static int laneX(int i) {
    int x = kLaneStartX;
    for (int j = 0; j < i; ++j) x += laneW(j) + kLaneSpacing;
    return x;
}
static int totalW(int n) {
    int w = 0;
    for (int i = 0; i < n; ++i) w += laneW(i) + (i > 0 ? kLaneSpacing : 0);
    return w;
}

// Colors
static SDL_Color laneBgCol(int i) {
    if (i == 0) return {20, 15, 30, 255};
    return (i % 2 == 1) ? SDL_Color{22,22,28,255} : SDL_Color{15,15,20,255};
}
static SDL_Color noteCol(int i) {
    if (i == 0) return {180, 100, 255, 255}; // turntable: purple
    return (i % 2 == 1) ? SDL_Color{230,230,230,255} : SDL_Color{100,160,255,255};
}
static SDL_Color keyCol(int i) {
    // Unpressed base color
    if (i == 0) return {60, 30, 90, 255};
    return (i % 2 == 1) ? SDL_Color{55,55,65,255} : SDL_Color{35,35,50,255};
}

static void sc(SDL_Renderer* r, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
}
static void fr(SDL_Renderer* r, int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    SDL_Rect rc{x,y,w,h}; SDL_RenderFillRect(r, &rc);
}
static void dr(SDL_Renderer* r, int x, int y, int w, int h) {
    SDL_Rect rc{x,y,w,h}; SDL_RenderDrawRect(r, &rc);
}

static void fillCircle(SDL_Renderer* r, int cx, int cy, int radius) {
    for (int dy = -radius; dy <= radius; ++dy) {
        int dx = static_cast<int>(std::sqrt(static_cast<double>(radius*radius - dy*dy)));
        SDL_RenderDrawLine(r, cx-dx, cy+dy, cx+dx, cy+dy);
    }
}

PlayerRenderer::PlayerRenderer() = default;
PlayerRenderer::~PlayerRenderer() { shutdown(); }

bool PlayerRenderer::init(int width, int height) {
    m_width = width; m_height = height;
    m_window = SDL_CreateWindow("iBMSC Preview",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!m_window) return false;
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) { SDL_DestroyWindow(m_window); m_window = nullptr; return false; }
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    return true;
}

void PlayerRenderer::shutdown() {
    if (m_renderer) { SDL_DestroyRenderer(m_renderer); m_renderer = nullptr; }
    if (m_window)   { SDL_DestroyWindow(m_window);     m_window   = nullptr; }
}

void PlayerRenderer::render(const PlayerChart& chart, double currentTime,
                            double scrollTime, double scrollSpeed,
                            const std::array<double, kRendererMaxLanes>& keyFade,
                            const std::array<double, kRendererMaxLanes>& hitEffect,
                            double ttRotation, int fps) {
    if (!m_renderer) return;

    int totalLanes = std::min(kRendererMaxLanes, chart.info.playerChannels + 1);
    int totW       = totalW(totalLanes);
    double pixPerSec = static_cast<double>(kLaneH) / scrollTime;

    // ── Clear ─────────────────────────────────────────────────────────────────
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    // ── BGA placeholder (right side, Pulsus: x=560, w=720) ───────────────────
    SDL_SetRenderDrawColor(m_renderer, 8, 8, 8, 255);
    fr(m_renderer, kBgaX, 0, kBgaW, kWinH);
    SDL_SetRenderDrawColor(m_renderer, 25, 25, 25, 255);
    dr(m_renderer, kBgaX, kLaneStartY, kBgaW - 10, kLaneH);

    // ── Lane backgrounds (above judge line only) ──────────────────────────────
    for (int i = 0; i < totalLanes; ++i) {
        sc(m_renderer, laneBgCol(i));
        fr(m_renderer, laneX(i), kLaneStartY, laneW(i), kLaneH);
    }

    // ── Clip to lane area so notes never bleed into HUD ───────────────────────
    SDL_Rect laneClip{kLaneStartX, kLaneStartY, totW, kLaneH};
    SDL_RenderSetClipRect(m_renderer, &laneClip);

    // ── Lane press glow (Pulsus: white * fade, 0.15s fadeout) ────────────────
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < totalLanes; ++i) {
        float fade = static_cast<float>(keyFade[i]);
        if (fade <= 0.0f) continue;
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, static_cast<Uint8>(60 * fade));
        fr(m_renderer, laneX(i) + 1, kLaneStartY, laneW(i) - 1, kLaneH);
    }

    // ── Measure lines (white 20% opacity) ────────────────────────────────────
    double bpm = chart.info.bpm > 0 ? chart.info.bpm : 130.0;
    double secPerMeasure = 60.0 / bpm * 4.0;
    double firstMT = std::floor(currentTime / secPerMeasure) * secPerMeasure;
    for (double mt = firstMT - secPerMeasure; mt < currentTime + scrollTime * 1.2; mt += secPerMeasure) {
        int y = kJudgeY - static_cast<int>((mt - currentTime) * pixPerSec);
        if (y < kLaneStartY || y > kJudgeY) continue;
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 51);
        SDL_RenderDrawLine(m_renderer, kLaneStartX, y, kLaneStartX + totW, y);
        SDL_RenderDrawLine(m_renderer, kLaneStartX, y + kNoteH/2, kLaneStartX + totW, y + kNoteH/2);
    }

    // ── Notes ─────────────────────────────────────────────────────────────────
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
    for (const auto& ev : chart.events) {
        if (ev.lane < 0 || ev.lane >= totalLanes) continue;
        double dt = ev.timeSec - currentTime;
        if (dt < 0.0 || dt > scrollTime * 1.2) continue;
        int lx = laneX(ev.lane), lw = laneW(ev.lane);
        SDL_Color nc = noteCol(ev.lane);
        int noteY = kJudgeY - static_cast<int>(dt * pixPerSec);

        if (ev.isLong) {
            double dtEnd = ev.endTimeSec - currentTime;
            int noteYEnd = kJudgeY - static_cast<int>(dtEnd * pixPerSec);
            int top = std::min(noteY, noteYEnd);
            int bot = std::max(noteY, noteYEnd);
            // LN body (dimmed, Pulsus uses LN texture variant)
            SDL_SetRenderDrawColor(m_renderer, nc.r/2, nc.g/2, nc.b/2, 200);
            fr(m_renderer, lx+3, top, lw-6, std::max(kNoteH, bot-top));
            // LN head and tail
            sc(m_renderer, nc);
            fr(m_renderer, lx, noteYEnd - kNoteH/2, lw, kNoteH);
            fr(m_renderer, lx, noteY    - kNoteH/2, lw, kNoteH);
        } else {
            sc(m_renderer, nc);
            fr(m_renderer, lx, noteY - kNoteH/2, lw, kNoteH);
        }
    }

    // ── Judge line (Pulsus: RGB(191,0,0), height=noteHeight) ─────────────────
    SDL_RenderSetClipRect(m_renderer, nullptr); // disable clip before judge line + HUD
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(m_renderer, 191, 0, 0, 255);
    fr(m_renderer, kLaneStartX, kJudgeY, totW, kNoteH);

    // ── Hit effects (100x100, centered on lane at judge line) ────────────────
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < totalLanes; ++i) {
        if (hitEffect[i] <= 0.0) continue;
        double progress = 1.0 - hitEffect[i] / (10.0 / 120.0);
        float alpha = static_cast<float>(1.0 - progress);
        int efW = static_cast<int>(60 + 40 * progress);
        int efH = efW;
        int cx  = laneX(i) + laneW(i) / 2;
        // Outer glow: PGreat color (light green, Pulsus RGB 176,230,176)
        SDL_SetRenderDrawColor(m_renderer, 176, 230, 176, static_cast<Uint8>(160 * alpha));
        fr(m_renderer, cx - efW/2, kJudgeY - efH/2, efW, efH);
        // Inner white core
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, static_cast<Uint8>(200 * alpha));
        fr(m_renderer, cx - efW/4, kJudgeY - efH/4, efW/2, efH/2);
    }

    // ── Key indicators (below judge line, Pulsus style) ───────────────────────
    // Black keys offset -5px vertically (Pulsus exact)
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
    for (int i = 1; i < totalLanes; ++i) {
        int lx = laneX(i), lw = laneW(i);
        int yOff = (i % 2 == 0) ? -5 : 0; // black keys offset
        float fade = static_cast<float>(keyFade[i]);
        // Base color
        SDL_Color base = keyCol(i);
        // Pressed: white→red fade (Pulsus: Color(1, fade, fade))
        Uint8 r = static_cast<Uint8>(base.r + (255 - base.r) * fade);
        Uint8 g = static_cast<Uint8>(base.g + (255 - base.g) * fade * (1.0f - fade * 0.8f));
        Uint8 b = static_cast<Uint8>(base.b + (255 - base.b) * fade * (1.0f - fade * 0.8f));
        SDL_SetRenderDrawColor(m_renderer, r, g, b, 255);
        fr(m_renderer, lx, kKeyAreaY + yOff, lw, 50);
        // Border
        SDL_SetRenderDrawColor(m_renderer, 50, 50, 60, 255);
        dr(m_renderer, lx, kKeyAreaY + yOff, lw, 50);
    }

    // ── Turntable (lane 0, Pulsus: circle with groove lines) ─────────────────
    if (totalLanes > 0) {
        int ttCX = laneX(0) + laneW(0) / 2;
        int ttCY = kKeyAreaY + 28;
        float fade0 = static_cast<float>(keyFade[0]);
        // Outer ring
        SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(m_renderer, 40, 25, 60, 255);
        fillCircle(m_renderer, ttCX, ttCY, kTTRadius);
        // Inner disc — Pulsus: Color(1, fade, fade) = red when pressed
        Uint8 tr = 160;
        Uint8 tg = static_cast<Uint8>(80  * (1.0f - fade0));
        Uint8 tb = static_cast<Uint8>(220 * (1.0f - fade0 * 0.6f));
        SDL_SetRenderDrawColor(m_renderer, tr, tg, tb, 255);
        fillCircle(m_renderer, ttCX, ttCY, kTTRadius - 4);
        // 3 groove lines rotating at 45 RPM
        SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(m_renderer, 220, 200, 255, 160);
        for (int g = 0; g < 3; ++g) {
            double angle = ttRotation + g * (2.0 * M_PI / 3.0);
            int x1 = ttCX + static_cast<int>(7  * std::cos(angle));
            int y1 = ttCY + static_cast<int>(7  * std::sin(angle));
            int x2 = ttCX + static_cast<int>((kTTRadius-5) * std::cos(angle));
            int y2 = ttCY + static_cast<int>((kTTRadius-5) * std::sin(angle));
            SDL_RenderDrawLine(m_renderer, x1, y1, x2, y2);
        }
        // Center dot
        SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        fr(m_renderer, ttCX-2, ttCY-2, 4, 4);
    }

    // ── HUD: speed + FPS (top-left above lanes) ───────────────────────────────
    {
        SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
        // Speed bar background
        SDL_SetRenderDrawColor(m_renderer, 15, 15, 15, 200);
        fr(m_renderer, kLaneStartX, 10, totW, 18);
        // Speed fill
        float frac = static_cast<float>(std::clamp(scrollSpeed, 0.5, 9.9) / 9.9);
        SDL_SetRenderDrawColor(m_renderer, 60, 180, 60, 220);
        fr(m_renderer, kLaneStartX + 1, 11, static_cast<int>((totW - 2) * frac), 16);
        // Tick marks at each integer speed
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 80);
        for (int t = 1; t < 10; ++t) {
            int tx = kLaneStartX + static_cast<int>((totW - 2) * t / 9.9f);
            SDL_RenderDrawLine(m_renderer, tx, 11, tx, 26);
        }
        // FPS indicator (right side of speed bar)
        // Draw as colored dot: green>=100, yellow>=60, red<60
        Uint8 fr_r = fps >= 100 ? 0   : fps >= 60 ? 200 : 255;
        Uint8 fr_g = fps >= 100 ? 200 : fps >= 60 ? 200 : 50;
        Uint8 fr_b = 0;
        SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(m_renderer, fr_r, fr_g, fr_b, 255);
        fr(m_renderer, kLaneStartX + totW + 4, 10, 8, 18);
    }

    SDL_RenderPresent(m_renderer);
}

} // namespace Player

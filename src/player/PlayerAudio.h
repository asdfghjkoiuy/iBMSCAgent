#pragma once
#include "audio/AudioEngine.h"
#include "player/PlayerChart.h"
#include <SDL2/SDL.h>
#include <vector>
#include <mutex>
#include <atomic>

namespace Player {

class PlayerAudio {
public:
    PlayerAudio();
    ~PlayerAudio();

    /// Initialize SDL audio. buffers = shared read-only pointer to AudioEngine's decoded PCM.
    bool init(const std::vector<Audio::SoundBuffer>* buffers);
    void shutdown();

    /// Schedule all events for playback.
    void loadEvents(std::vector<PlayEvent> events);

    void play();
    void stop();
    bool isPlaying() const { return m_playing.load(); }
    double position() const { return m_position.load(); }

private:
    static void audioCallback(void* userdata, Uint8* stream, int len);
    void mix(int16_t* output, int frames);
    void triggerSlot(int slotIndex);

    SDL_AudioDeviceID m_device = 0;
    SDL_AudioSpec m_spec{};
    const std::vector<Audio::SoundBuffer>* m_buffers = nullptr;

    struct ActiveSound {
        int bufferIndex = -1;
        int position = 0;
    };

    std::vector<ActiveSound> m_active;
    std::mutex m_mutex;

    std::vector<PlayEvent> m_events;
    size_t m_eventCursor = 0;

    std::atomic<bool> m_playing{false};
    std::atomic<double> m_position{0.0};
    double m_startTicks = 0.0;
};

} // namespace Player

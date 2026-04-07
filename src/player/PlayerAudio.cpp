#include "player/PlayerAudio.h"
#include <cstring>
#include <algorithm>

namespace Player {

PlayerAudio::PlayerAudio() = default;

PlayerAudio::~PlayerAudio() { shutdown(); }

bool PlayerAudio::init(const std::vector<Audio::SoundBuffer>* buffers) {
    m_buffers = buffers;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) return false;

    SDL_AudioSpec desired{};
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 1024;
    desired.callback = audioCallback;
    desired.userdata = this;

    m_device = SDL_OpenAudioDevice(nullptr, 0, &desired, &m_spec, 0);
    return m_device != 0;
}

void PlayerAudio::shutdown() {
    stop();
    if (m_device) {
        SDL_CloseAudioDevice(m_device);
        m_device = 0;
    }
}

void PlayerAudio::loadEvents(std::vector<PlayEvent> events) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events = std::move(events);
    m_eventCursor = 0;
}

void PlayerAudio::play() {
    m_playing = true;
    m_startTicks = static_cast<double>(SDL_GetPerformanceCounter());
    m_eventCursor = 0;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_active.clear();
    }
    SDL_PauseAudioDevice(m_device, 0);
}

void PlayerAudio::stop() {
    if (m_device) SDL_PauseAudioDevice(m_device, 1);
    m_playing = false;
    std::lock_guard<std::mutex> lock(m_mutex);
    m_active.clear();
}

void PlayerAudio::triggerSlot(int slotIndex) {
    if (!m_buffers || slotIndex < 0 || slotIndex >= static_cast<int>(m_buffers->size()))
        return;
    if (!(*m_buffers)[slotIndex].valid) return;
    // Limit concurrent sounds
    if (m_active.size() >= 64) return;
    m_active.push_back({slotIndex, 0});
}

void PlayerAudio::audioCallback(void* userdata, Uint8* stream, int len) {
    auto* self = static_cast<PlayerAudio*>(userdata);
    int frames = len / (2 * sizeof(int16_t)); // stereo int16
    std::memset(stream, 0, len);

    if (!self->m_playing) return;

    double now = (static_cast<double>(SDL_GetPerformanceCounter()) - self->m_startTicks)
                 / SDL_GetPerformanceFrequency();
    self->m_position = now;

    // Trigger scheduled events
    {
        std::lock_guard<std::mutex> lock(self->m_mutex);
        while (self->m_eventCursor < self->m_events.size()) {
            const auto& ev = self->m_events[self->m_eventCursor];
            if (ev.timeSec > now + 0.01) break; // 10ms lookahead
            self->triggerSlot(ev.slotIndex);
            ++self->m_eventCursor;
        }
    }

    self->mix(reinterpret_cast<int16_t*>(stream), frames);
}

void PlayerAudio::mix(int16_t* output, int frames) {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto it = m_active.begin(); it != m_active.end(); ) {
        const auto& buf = (*m_buffers)[it->bufferIndex];
        const int16_t* pcm = reinterpret_cast<const int16_t*>(buf.pcm.constData());
        int totalSamples = buf.pcm.size() / sizeof(int16_t);
        int samplesPerFrame = buf.channels;

        for (int f = 0; f < frames && it->position < totalSamples / samplesPerFrame; ++f) {
            int idx = it->position * samplesPerFrame;
            int left = pcm[idx];
            int right = (samplesPerFrame >= 2) ? pcm[idx + 1] : left;
            output[f * 2]     = static_cast<int16_t>(std::clamp(output[f * 2] + left, -32768, 32767));
            output[f * 2 + 1] = static_cast<int16_t>(std::clamp(output[f * 2 + 1] + right, -32768, 32767));
            ++it->position;
        }

        if (it->position >= totalSamples / samplesPerFrame)
            it = m_active.erase(it);
        else
            ++it;
    }
}

} // namespace Player

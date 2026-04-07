#include "audio/AudioEngine.h"
#include "model/TimingMap.h"
#include "model/ChannelType.h"

// miniaudio implementation
#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#include "audio/miniaudio.h"

#include <QAudioDevice>
#include <QAudioBuffer>
#include <QMediaDevices>
#include <QAudioDecoder>
#include <QIODevice>
#include <QEventLoop>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QDateTime>
#include <algorithm>
#include <cstring>
#include <cmath>

namespace Audio {

using namespace Model;

namespace {

SoundBuffer decodeAudioFileQt(const QString& filename, const QAudioFormat& targetFormat) {
    SoundBuffer out{};
    if (!QFile::exists(filename)) {
        qWarning() << "[AudioEngine] File not found:" << filename;
        return out;
    }

    QAudioDecoder decoder;
    decoder.setAudioFormat(targetFormat);
    decoder.setSource(QUrl::fromLocalFile(filename));

    QEventLoop loop;
    bool finished = false;
    bool hadError = false;
    QObject::connect(&decoder, &QAudioDecoder::finished, &loop, [&]() { finished = true; loop.quit(); });
    QObject::connect(&decoder, qOverload<QAudioDecoder::Error>(&QAudioDecoder::error), &loop, [&]() {
        hadError = true;
        qWarning() << "[AudioEngine] Decode error for" << filename << ":" << decoder.errorString();
        loop.quit();
    });

    QByteArray pcm;
    QObject::connect(&decoder, &QAudioDecoder::bufferReady, &decoder, [&]() {
        QAudioBuffer buf = decoder.read();
        if (!buf.isValid()) return;
        const QAudioFormat fmt = buf.format();
        const int channels = std::max(1, fmt.channelCount());

        if (fmt.sampleFormat() == QAudioFormat::Int16) {
            const int16_t* src = buf.constData<int16_t>();
            const int sampleCount = buf.sampleCount();
            pcm.append(reinterpret_cast<const char*>(src), sampleCount * static_cast<int>(sizeof(int16_t)));
        } else if (fmt.sampleFormat() == QAudioFormat::Float) {
            const float* src = buf.constData<float>();
            const int sampleCount = buf.sampleCount();
            const int oldSize = pcm.size();
            pcm.resize(oldSize + sampleCount * static_cast<int>(sizeof(int16_t)));
            int16_t* dst = reinterpret_cast<int16_t*>(pcm.data() + oldSize);
            for (int i = 0; i < sampleCount; ++i) {
                float v = std::clamp(src[i], -1.0f, 1.0f);
                dst[i] = static_cast<int16_t>(v * 32767.0f);
            }
        }

        out.channels = channels;
        out.sampleRate = fmt.sampleRate() > 0 ? fmt.sampleRate() : targetFormat.sampleRate();
    });

    decoder.start();
    QTimer::singleShot(5000, &loop, &QEventLoop::quit);
    loop.exec();

    if (!pcm.isEmpty()) {
        out.pcm = std::move(pcm);
        out.valid = true;
        if (out.channels <= 0) out.channels = 2;
        if (out.sampleRate <= 0) out.sampleRate = targetFormat.sampleRate();
    } else {
        qWarning() << "[AudioEngine] No PCM decoded from" << filename;
    }
    return out;
}

std::vector<float> computePeaks(const SoundBuffer& buf, int bins = 2048) {
    std::vector<float> peaks;
    if (!buf.valid || buf.channels <= 0 || buf.pcm.isEmpty()) return peaks;
    const int16_t* src = reinterpret_cast<const int16_t*>(buf.pcm.constData());
    const int totalSamples = buf.pcm.size() / static_cast<int>(sizeof(int16_t));
    const int totalFrames = totalSamples / buf.channels;
    if (totalFrames <= 0) return peaks;
    bins = std::clamp(bins, 256, 8192);
    peaks.assign(static_cast<size_t>(bins), 0.0f);
    for (int i = 0; i < bins; ++i) {
        int startFrame = static_cast<int>((static_cast<long long>(i) * totalFrames) / bins);
        int endFrame = std::max(startFrame + 1, static_cast<int>((static_cast<long long>(i + 1) * totalFrames) / bins));
        int16_t maxAbs = 0;
        for (int f = startFrame; f < endFrame; ++f) {
            int base = f * buf.channels;
            int16_t sample = src[base];
            if (buf.channels > 1) sample = static_cast<int16_t>((static_cast<int>(src[base]) + static_cast<int>(src[base + 1])) / 2);
            int16_t a = static_cast<int16_t>(std::abs(static_cast<int>(sample)));
            if (a > maxAbs) maxAbs = a;
        }
        peaks[static_cast<size_t>(i)] = static_cast<float>(maxAbs) / 32767.0f;
    }
    return peaks;
}

} // namespace

// ── miniaudio callback ────────────────────────────────────────────────────────

static void maCallback(ma_device* device, void* output, const void*, ma_uint32 frameCount) {
    auto* engine = static_cast<AudioEngine*>(device->pUserData);
    if (engine) engine->maDataCallback(output, static_cast<int>(frameCount));
}

// ─────────────────────────────────────────────────────────────────────────────

AudioEngine::AudioEngine(QObject* parent) : QObject(parent) {
    m_format.setSampleRate(44100);
    m_format.setChannelCount(2);
    m_format.setSampleFormat(QAudioFormat::Int16);

    m_buffers.resize(kMaxSlots);
    m_slotWaveformPeaks.resize(kMaxSlots);
    m_maDevice = std::make_unique<ma_device>();

    connect(&m_positionTimer, &QTimer::timeout, this, &AudioEngine::onPositionTimer);
    m_positionTimer.setInterval(16);

    // Defer miniaudio init until the event loop is running — CoreAudio requires
    // the main run loop to be active before audio device creation on macOS.
    QTimer::singleShot(0, this, [this]() { initMiniAudio(); });
}

AudioEngine::~AudioEngine() {
    m_positionTimer.stop();
    if (ma_device_is_started(m_maDevice.get()))
        ma_device_stop(m_maDevice.get());
    ma_device_uninit(m_maDevice.get());
}

void AudioEngine::initMiniAudio() {
    // If already running, just return — avoid reinit delay
    if (ma_device_is_started(m_maDevice.get())) return;

    if (ma_device_get_state(m_maDevice.get()) != ma_device_state_uninitialized) {
        ma_device_uninit(m_maDevice.get());
    }

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_s16;
    config.playback.channels = 2;
    config.sampleRate        = 44100;
    config.dataCallback      = maCallback;
    config.pUserData         = this;
    config.periodSizeInMilliseconds = 5; // ~5ms latency

    if (ma_device_init(nullptr, &config, m_maDevice.get()) != MA_SUCCESS) {
        qWarning() << "[AudioEngine] miniaudio init failed";
        return;
    }
    if (ma_device_start(m_maDevice.get()) != MA_SUCCESS) {
        qWarning() << "[AudioEngine] miniaudio start failed";
        return;
    }
    qDebug() << "[AudioEngine] miniaudio started, period=" << config.periodSizeInMilliseconds << "ms";
}

void AudioEngine::maDataCallback(void* output, int frameCount) {
    int16_t* out = static_cast<int16_t*>(output);
    std::memset(out, 0, static_cast<size_t>(frameCount) * 2 * sizeof(int16_t));

    // Trigger scheduled events
    if (m_isPlaying) {
        double now = static_cast<double>(QDateTime::currentMSecsSinceEpoch()) / 1000.0;
        double pos = now - m_playbackStart.load();
        m_playbackPosition = pos;

        QMutexLocker lock(&m_scheduleMutex);
        auto it = m_schedule.begin();
        while (it != m_schedule.end() && it->triggerTime <= pos) {
            triggerSlot(it->slotIndex);
            ++it;
        }
        m_schedule.erase(m_schedule.begin(), it);
    }

    mixInto(out, frameCount);
}

QStringList AudioEngine::deviceNames() const {
    QStringList names;
    for (const auto& d : QMediaDevices::audioOutputs())
        names << d.description();
    return names;
}

void AudioEngine::setDevice(const QString& deviceName) {
    m_deviceName = deviceName;
    initMiniAudio();
    emit deviceNamesChanged();
}

void AudioEngine::loadKeysounds(const QString& basePath, const BmsDocument& doc) {
    m_globalPreviewSlot = -1;
    int loaded = 0, failed = 0;
    for (int i = 1; i < kMaxSlots; ++i) {
        if (doc.wavTable[i].empty()) { m_buffers[i] = {}; m_slotWaveformPeaks[i].clear(); continue; }
        QString filename = QDir(basePath).filePath(QString::fromStdString(doc.wavTable[i]));
        if (!QFile::exists(filename)) {
            bool found = false;
            for (const char* ext : {".wav", ".ogg", ".mp3"}) {
                QString tryName = QDir(basePath).filePath(QString::fromStdString(doc.wavTable[i]).section('.', 0, 0) + ext);
                if (QFile::exists(tryName)) { filename = tryName; found = true; break; }
            }
            if (!found) { m_buffers[i] = {}; m_slotWaveformPeaks[i].clear(); ++failed; continue; }
        }
        SoundBuffer decoded = decodeAudioFileQt(filename, m_format);
        m_buffers[i] = std::move(decoded);
        updateWaveformPeaks(i);
        if (m_buffers[i].valid) { ++loaded; if (m_globalPreviewSlot < 0) m_globalPreviewSlot = i; }
        else ++failed;
    }
    qDebug() << "[AudioEngine] loadKeysounds:" << loaded << "loaded," << failed << "failed";
    emit waveformChanged();
}

void AudioEngine::setShowWaveform(bool v) {
    if (m_showWaveform == v) return;
    m_showWaveform = v;
    emit waveformChanged();
}

QVariantList AudioEngine::waveformPeaksForSlot(int slotIndex) const {
    QVariantList out;
    if (slotIndex < 1 || slotIndex >= kMaxSlots) return out;
    for (float v : m_slotWaveformPeaks[slotIndex]) out.push_back(v);
    return out;
}

QVariantList AudioEngine::globalWaveformPeaks() const {
    QVariantList out;
    if (m_globalPreviewSlot < 0 || m_globalPreviewSlot >= kMaxSlots) return out;
    for (float v : m_slotWaveformPeaks[m_globalPreviewSlot]) out.push_back(v);
    return out;
}

const std::vector<float>& AudioEngine::slotWaveformPeaks(int slotIndex) const {
    static const std::vector<float> kEmpty;
    if (slotIndex < 1 || slotIndex >= kMaxSlots) return kEmpty;
    return m_slotWaveformPeaks[slotIndex];
}

void AudioEngine::updateWaveformPeaks(int slotIndex) {
    if (slotIndex < 1 || slotIndex >= kMaxSlots) return;
    m_slotWaveformPeaks[slotIndex] = computePeaks(m_buffers[slotIndex]);
}

void AudioEngine::play(int slotIndex) {
    if (slotIndex < 1 || slotIndex >= kMaxSlots) return;
    if (!m_buffers[slotIndex].valid) return;
    triggerSlot(slotIndex);
    if (!ma_device_is_started(m_maDevice.get())) initMiniAudio();
}

void AudioEngine::triggerSlot(int slotIndex) {
    QMutexLocker lock(&m_activeMutex);
    if (m_active.size() >= 64) m_active.erase(m_active.begin());
    m_active.push_back({slotIndex, 0});
}

void AudioEngine::scheduleChart(const BmsDocument& doc, double startTimeSec) {
    m_isPlaying = false;
    m_positionTimer.stop();
    {
        QMutexLocker lock(&m_scheduleMutex);
        m_schedule.clear();
    }
    {
        QMutexLocker lock(&m_activeMutex);
        m_active.clear();
    }

    TimingMap tmap(doc);
    std::vector<ScheduledEvent> events;
    for (const auto& note : doc.notes) {
        ChannelType ct = classifyChannel(note.channelIndex);
        if (ct != ChannelType::BGM && !isPlayerNoteChannel(ct)) continue;
        if (note.value < 1 || note.value >= kMaxSlots) continue;
        double t = tmap.beatToSeconds(note.measureIndex, note.beat.toDouble() * doc.measureLength(note.measureIndex));
        if (t < startTimeSec) continue;
        events.push_back({t - startTimeSec, note.value});
    }
    std::sort(events.begin(), events.end(), [](const ScheduledEvent& a, const ScheduledEvent& b) { return a.triggerTime < b.triggerTime; });
    {
        QMutexLocker lock(&m_scheduleMutex);
        m_schedule = std::move(events);
    }

    qDebug() << "[AudioEngine] scheduleChart:" << m_schedule.size() << "events";

    m_playbackPosition = 0.0;
    m_isPlaying = true;
    emit isPlayingChanged();

    // Init audio FIRST, then record start time to avoid timing offset
    initMiniAudio();
    m_playbackStart = static_cast<double>(QDateTime::currentMSecsSinceEpoch()) / 1000.0;
    qDebug() << "[AudioEngine] scheduleChart: ma_device started=" << ma_device_is_started(m_maDevice.get());
    m_positionTimer.start();
}

void AudioEngine::stop() {
    m_isPlaying = false;
    m_positionTimer.stop();
    {
        QMutexLocker lock(&m_scheduleMutex);
        m_schedule.clear();
    }
    {
        QMutexLocker lock(&m_activeMutex);
        m_active.clear();
    }
    emit isPlayingChanged();
}

void AudioEngine::onPositionTimer() {
    if (!m_isPlaying) return;
    double pos = m_playbackPosition.load();
    {
        QMutexLocker lock(&m_scheduleMutex);
        if (m_schedule.empty()) {
            QMutexLocker lock2(&m_activeMutex);
            if (m_active.empty()) {
                m_isPlaying = false;
                m_positionTimer.stop();
                emit isPlayingChanged();
            }
        }
    }
    emit positionChanged(pos);
}

void AudioEngine::mixInto(int16_t* output, int frameCount) {
    QMutexLocker lock(&m_activeMutex);
    for (auto& inst : m_active) {
        if (inst.bufferIndex < 0 || inst.bufferIndex >= kMaxSlots) continue;
        const SoundBuffer& buf = m_buffers[inst.bufferIndex];
        if (!buf.valid) continue;
        const int16_t* src = reinterpret_cast<const int16_t*>(buf.pcm.constData());
        int totalSamples = buf.pcm.size() / sizeof(int16_t);
        int srcChannels = buf.channels;
        for (int frame = 0; frame < frameCount; ++frame) {
            if (inst.position >= totalSamples) break;
            int32_t l = output[frame * 2];
            int32_t r = output[frame * 2 + 1];
            if (srcChannels == 1) {
                int16_t s = src[inst.position];
                l += s; r += s;
            } else {
                l += src[inst.position];
                r += src[inst.position + 1];
            }
            output[frame * 2]     = static_cast<int16_t>(std::clamp(l, -32768, 32767));
            output[frame * 2 + 1] = static_cast<int16_t>(std::clamp(r, -32768, 32767));
            inst.position += srcChannels;
        }
    }
    m_active.erase(std::remove_if(m_active.begin(), m_active.end(), [&](const SoundInstance& inst) {
        if (inst.bufferIndex < 0 || inst.bufferIndex >= kMaxSlots) return true;
        return inst.position >= m_buffers[inst.bufferIndex].pcm.size() / (int)sizeof(int16_t);
    }), m_active.end());
}

} // namespace Audio

#pragma once
#include "model/BmsDocument.h"
#include <QObject>
#include <QAudioFormat>
#include <QTimer>
#include <QMutex>
#include <QByteArray>
#include <QVariantList>
#include <vector>
#include <memory>
#include <atomic>

// Forward declare ma_device to avoid including miniaudio.h in header
struct ma_device;

namespace Audio {

struct SoundBuffer {
    QByteArray pcm;
    int channels = 2;
    int sampleRate = 44100;
    bool valid = false;
};

struct SoundInstance {
    int bufferIndex = -1;
    int position = 0;
};

struct ScheduledEvent {
    double triggerTime = 0.0;
    int slotIndex = 0;
};

class AudioEngine : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList deviceNames READ deviceNames NOTIFY deviceNamesChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(double playbackPosition READ playbackPosition NOTIFY positionChanged)
    Q_PROPERTY(bool showWaveform READ showWaveform WRITE setShowWaveform NOTIFY waveformChanged)
    Q_PROPERTY(QVariantList globalWaveformPeaks READ globalWaveformPeaks NOTIFY waveformChanged)

public:
    explicit AudioEngine(QObject* parent = nullptr);
    ~AudioEngine() override;

    Q_INVOKABLE void loadKeysounds(const QString& basePath, const Model::BmsDocument& doc);
    Q_INVOKABLE void play(int slotIndex);
    Q_INVOKABLE void scheduleChart(const Model::BmsDocument& doc, double startTimeSec = 0.0);
    Q_INVOKABLE void stop();

    QStringList deviceNames() const;
    bool isPlaying() const { return m_isPlaying; }
    double playbackPosition() const { return m_playbackPosition; }
    bool showWaveform() const { return m_showWaveform; }

    void setShowWaveform(bool v);
    QVariantList globalWaveformPeaks() const;
    const std::vector<float>& slotWaveformPeaks(int slotIndex) const;
    int globalPreviewSlot() const { return m_globalPreviewSlot; }

    /// Read-only access to decoded PCM buffers (for player sharing).
    const std::vector<SoundBuffer>* soundBuffers() const { return &m_buffers; }

    Q_INVOKABLE void setDevice(const QString& deviceName);
    Q_INVOKABLE QVariantList waveformPeaksForSlot(int slotIndex) const;

    // Called from miniaudio callback thread — must be thread-safe
    void maDataCallback(void* output, int frameCount);

signals:
    void deviceNamesChanged();
    void isPlayingChanged();
    void positionChanged(double seconds);
    void waveformChanged();

private slots:
    void onPositionTimer();

private:
    void initMiniAudio();
    void triggerSlot(int slotIndex);
    void updateWaveformPeaks(int slotIndex);
    void mixInto(int16_t* output, int frameCount);

    QAudioFormat m_format;
    std::unique_ptr<ma_device> m_maDevice;

    std::vector<SoundBuffer> m_buffers;
    std::vector<SoundInstance> m_active;
    QMutex m_activeMutex;

    std::vector<ScheduledEvent> m_schedule;
    QMutex m_scheduleMutex;
    std::atomic<double> m_playbackStart{0.0};
    std::atomic<bool> m_isPlaying{false};
    std::atomic<double> m_playbackPosition{0.0};

    QTimer m_positionTimer;
    QString m_deviceName;
    std::vector<std::vector<float>> m_slotWaveformPeaks;
    bool m_showWaveform = true;
    int m_globalPreviewSlot = -1;
};

} // namespace Audio

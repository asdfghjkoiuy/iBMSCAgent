#include "audio/audio_preview_service.h"

#include <QAudioOutput>
#include <QDir>
#include <QFileInfo>
#include <QMediaPlayer>

namespace ibmsc {

AudioPreviewService::AudioPreviewService(QObject* parent)
    : QObject(parent), m_player(new QMediaPlayer(this)), m_output(new QAudioOutput(this)) {
    m_output->setVolume(0.9);
    m_player->setAudioOutput(m_output);
}

AudioPreviewService::~AudioPreviewService() = default;

QString AudioPreviewService::resolvePlayablePath(const QString& path, const QString& rootDir) const {
    QFileInfo fi(path);
    if (!fi.isAbsolute()) {
        fi = QFileInfo(QDir(rootDir), path);
    }
    if (fi.exists()) {
        return fi.absoluteFilePath();
    }

    const QString ext = fi.suffix().toLower();
    QString alternative = fi.absoluteFilePath();
    if (ext == "wav") {
        alternative.chop(4);
        alternative += ".ogg";
    } else if (ext == "ogg") {
        alternative.chop(4);
        alternative += ".wav";
    }
    if (!alternative.isEmpty() && QFileInfo::exists(alternative)) {
        return alternative;
    }
    return QString();
}

void AudioPreviewService::play(const QString& absoluteOrRelativePath, const QString& rootDir) {
    const QString resolved = resolvePlayablePath(absoluteOrRelativePath, rootDir);
    if (resolved.isEmpty()) {
        return;
    }
    m_player->setSource(QUrl::fromLocalFile(resolved));
    m_player->play();
}

void AudioPreviewService::stop() {
    m_player->stop();
}

} // namespace ibmsc

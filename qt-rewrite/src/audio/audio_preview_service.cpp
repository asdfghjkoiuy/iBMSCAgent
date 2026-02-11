#include "audio/audio_preview_service.h"

#include <QAudioOutput>
#include <QDir>
#include <QFileInfo>
#include <QMediaPlayer>
#include <algorithm>

namespace ibmsc {

namespace {
QString normalizeAudioRef(QString in) {
    in = in.trimmed();
    if (in.startsWith('"') && in.endsWith('"') && in.size() >= 2) {
        in = in.mid(1, in.size() - 2);
    }
    if (in.startsWith('\'') && in.endsWith('\'') && in.size() >= 2) {
        in = in.mid(1, in.size() - 2);
    }
    in = QDir::fromNativeSeparators(in);
    while (in.startsWith("./")) {
        in.remove(0, 2);
    }
    while (in.startsWith('/')) {
        in.remove(0, 1);
    }
    return in.trimmed();
}

QString findCaseInsensitive(const QFileInfo& probe) {
    const QDir dir = probe.dir();
    if (!dir.exists()) {
        return QString();
    }
    const QString target = probe.fileName().toLower();
    const QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QFileInfo& e : entries) {
        if (e.fileName().toLower() == target) {
            return e.absoluteFilePath();
        }
    }
    return QString();
}
} // namespace

AudioPreviewService::AudioPreviewService(QObject* parent)
    : QObject(parent), m_player(new QMediaPlayer(this)), m_output(new QAudioOutput(this)) {
    m_output->setVolume(0.9);
    m_player->setAudioOutput(m_output);
}

AudioPreviewService::~AudioPreviewService() = default;

QString AudioPreviewService::resolvePlayablePath(const QString& path, const QString& rootDir) {
    const QString normalizedRoot = QDir::cleanPath(QDir::fromNativeSeparators(rootDir));
    const QString normalizedInput = normalizeAudioRef(path);
    if (normalizedInput.isEmpty()) {
        return QString();
    }

    const QString cacheKey = normalizedRoot.toLower() + "|" + normalizedInput.toLower();
    if (m_resolveCache.contains(cacheKey)) {
        return m_resolveCache.value(cacheKey);
    }

    QFileInfo fi(normalizedInput);
    if (!fi.isAbsolute()) {
        fi = QFileInfo(QDir(normalizedRoot), normalizedInput);
    }
    if (fi.exists()) {
        const QString resolved = fi.absoluteFilePath();
        m_resolveCache.insert(cacheKey, resolved);
        return resolved;
    }

    {
        const QString matched = findCaseInsensitive(fi);
        if (!matched.isEmpty()) {
            m_resolveCache.insert(cacheKey, matched);
            return matched;
        }
    }

    const QString ext = fi.suffix().toLower();
    QStringList candidates;
    const QString baseNoExt = fi.absolutePath() + "/" + fi.completeBaseName();
    if (ext == "wav") {
        candidates << (baseNoExt + ".ogg");
    } else if (ext == "ogg") {
        candidates << (baseNoExt + ".wav");
    } else if (ext.isEmpty()) {
        candidates << (fi.absoluteFilePath() + ".wav")
                   << (fi.absoluteFilePath() + ".ogg");
    }

    QFileInfo underWav(QDir(normalizedRoot + "/wav"), fi.fileName());
    QFileInfo underWAV(QDir(normalizedRoot + "/WAV"), fi.fileName());
    candidates << underWav.absoluteFilePath() << underWAV.absoluteFilePath();
    if (ext.isEmpty()) {
        candidates << (underWav.absoluteFilePath() + ".wav")
                   << (underWav.absoluteFilePath() + ".ogg")
                   << (underWAV.absoluteFilePath() + ".wav")
                   << (underWAV.absoluteFilePath() + ".ogg");
    }

    for (const QString& c : candidates) {
        if (c.isEmpty()) {
            continue;
        }
        const QFileInfo cfi(c);
        if (cfi.exists()) {
            const QString resolved = cfi.absoluteFilePath();
            m_resolveCache.insert(cacheKey, resolved);
            return resolved;
        }
        const QString matched = findCaseInsensitive(cfi);
        if (!matched.isEmpty()) {
            m_resolveCache.insert(cacheKey, matched);
            return matched;
        }
    }

    // Last fallback: same file name search under chart root and wav folders.
    const QString targetName = fi.fileName().toLower();
    const QStringList roots = {normalizedRoot, normalizedRoot + "/wav", normalizedRoot + "/WAV"};
    for (const QString& root : roots) {
        const QDir dir(root);
        if (!dir.exists()) {
            continue;
        }
        const QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::Readable | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                                                      QDir::Name);
        for (const QFileInfo& f : files) {
            if (f.fileName().toLower() == targetName) {
                const QString resolved = f.absoluteFilePath();
                m_resolveCache.insert(cacheKey, resolved);
                return resolved;
            }
        }
    }

    m_resolveCache.insert(cacheKey, QString());
    return QString();
}

void AudioPreviewService::play(const QString& absoluteOrRelativePath, const QString& rootDir) {
    const QString resolved = resolvePlayablePath(absoluteOrRelativePath, rootDir);
    if (resolved.isEmpty()) {
        return;
    }
    if (m_currentSourcePath != resolved) {
        m_currentSourcePath = resolved;
        m_player->setSource(QUrl::fromLocalFile(resolved));
    } else {
        m_player->setPosition(0);
    }
    m_player->play();
}

void AudioPreviewService::stop() {
    m_player->stop();
}

} // namespace ibmsc

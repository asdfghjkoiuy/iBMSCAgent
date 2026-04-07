#include "app/AppController.h"
#include "app/AppSettings.h"
#include "editor/EditorDocument.h"
#include "audio/AudioEngine.h"
#include "io/BmsParser.h"
#include "io/BmsSerializer.h"
#include "editor/commands/BpmToolsCommands.h"
#include "player/PlayerBridge.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <cmath>

namespace App {

AppController::AppController(Editor::EditorDocument* doc,
                              Audio::AudioEngine* audio,
                              QObject* parent)
    : QObject(parent), m_doc(doc), m_audio(audio) {}

AppController::~AppController() = default;

bool AppController::hasDocument() const {
    return m_hasActiveDocument;
}

void AppController::newFile() {
    m_doc->resetDocument({}, {});
    m_hasActiveDocument = true;
    emit hasDocumentChanged();
}

void AppController::closeFile() {
    m_doc->resetDocument({}, {});
    m_doc->setFilePath("");
    m_hasActiveDocument = false;
    emit hasDocumentChanged();
}

void AppController::openFile(const QString& path) {
    if (!path.isEmpty()) doOpen(path);
}

void AppController::requestOpenDialog() {
    emit openDialogRequested();
}

void AppController::doOpen(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        emit errorOccurred("Cannot open file: " + path);
        return;
    }
    auto result = IO::BmsParser::parse(f);
    m_doc->resetDocument(std::move(result.doc), path);

    QString dir = QFileInfo(path).absolutePath();
    // Load keysounds in a safe way — don't crash if audio fails
    try {
        m_audio->loadKeysounds(dir, m_doc->document());
    } catch (...) {
        qWarning() << "[AppController] loadKeysounds failed for" << dir;
    }
    AppSettings::instance()->addRecentFile(path);
    m_hasActiveDocument = true;
    emit hasDocumentChanged();
}

bool AppController::saveFile() {
    if (m_doc->filePath().isEmpty()) {
        emit saveAsDialogRequested({});
        return false; // QML will call saveFileTo() after dialog
    }
    return doSave(m_doc->filePath());
}

void AppController::requestSaveAsDialog() {
    emit saveAsDialogRequested(m_doc->filePath());
}

bool AppController::saveFileTo(const QString& path) {
    if (path.isEmpty()) return false;
    return doSave(path);
}

bool AppController::doSave(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        emit errorOccurred("Cannot write file: " + path);
        return false;
    }
    bool ok = IO::BmsSerializer::save(m_doc->document(), f);
    if (ok) {
        m_doc->setFilePath(path);
        m_doc->markClean();
        AppSettings::instance()->addRecentFile(path);
    }
    return ok;
}

void AppController::playFromPosition(double startTimeSec) {
    m_audio->scheduleChart(m_doc->document(), startTimeSec);
}

void AppController::stopPlayback() {
    m_audio->stop();
}

bool AppController::convertWavSlotToOgg(int slotIndex) {
    if (slotIndex < 1 || slotIndex >= Model::kMaxSlots) return false;

    const auto& doc = m_doc->document();
    QString rel = QString::fromStdString(doc.wavTable[slotIndex]);
    if (rel.isEmpty()) return false;

    QString baseDir = QFileInfo(m_doc->filePath()).absolutePath();
    QString inputPath = QDir(baseDir).filePath(rel);
    QFileInfo inInfo(inputPath);
    if (!inInfo.exists()) {
        emit errorOccurred("Missing source audio: " + inputPath);
        return false;
    }

    QString outPath = inInfo.path() + "/" + inInfo.completeBaseName() + ".ogg";
    QString program = "ffmpeg";
    QStringList args = {"-y", "-i", inputPath, "-c:a", "libvorbis", "-q:a", "5", outPath};
    int code = QProcess::execute(program, args);
    if (code != 0) {
        emit errorOccurred("ffmpeg conversion failed. Ensure ffmpeg is installed and in PATH.");
        return false;
    }

    m_doc->setWavFilename(slotIndex, QFileInfo(outPath).fileName());
    m_audio->loadKeysounds(baseDir, m_doc->document());
    return true;
}

bool AppController::importAudioToBgmLane(const QString& localFilePath, int channelIndex, double absoluteBeat) {
    QFileInfo srcInfo(localFilePath);
    if (!srcInfo.exists()) return false;

    QString baseDir = QFileInfo(m_doc->filePath()).absolutePath();
    if (baseDir.isEmpty()) baseDir = srcInfo.absolutePath();

    QString destName = srcInfo.fileName();
    QString destPath = QDir(baseDir).filePath(destName);
    if (QFileInfo(localFilePath).absoluteFilePath() != QFileInfo(destPath).absoluteFilePath()) {
        QFile::copy(localFilePath, destPath);
    }

    int slot = -1;
    for (int i = 1; i < Model::kMaxSlots; ++i) {
        if (m_doc->document().wavTable[i].empty()) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        emit errorOccurred("No free WAV slot available.");
        return false;
    }

    m_doc->setWavFilename(slot, destName);

    int measure = 0;
    double remain = std::max(0.0, absoluteBeat);
    while (true) {
        double len = m_doc->document().measureLength(measure) * 4.0;
        if (remain < len - 1e-9) break;
        remain -= len;
        ++measure;
    }
    int den = 192;
    double norm = remain / std::max(1e-9, m_doc->document().measureLength(measure) * 4.0);
    int num = static_cast<int>(std::round(norm * den));

    m_doc->placeNote(channelIndex, measure, num, den, slot);
    m_audio->loadKeysounds(baseDir, m_doc->document());
    return true;
}

void AppController::constBpm(int targetBpm) {
    if (targetBpm < 1 || targetBpm > 9999) return;
    m_doc->undoStack()->push(new Editor::Commands::ConstBpmCommand(m_doc, static_cast<double>(targetBpm)));
}

void AppController::scaleBpm(double ratio) {
    if (ratio <= 0.0 || ratio > 100.0) return;
    m_doc->undoStack()->push(new Editor::Commands::ScaleBpmCommand(m_doc, ratio));
}

void AppController::launchPreview(int startMeasure) {
    if (!m_player)
        m_player = std::make_unique<Player::PlayerBridge>(this);
    m_player->launchPreview(m_doc->document(), m_audio, startMeasure);
}

void AppController::closePreview() {
    if (m_player) m_player->closePreview();
}

} // namespace App

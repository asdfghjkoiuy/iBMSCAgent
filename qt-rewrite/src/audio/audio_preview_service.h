#pragma once

#include <QObject>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QAudioOutput;
class QMediaPlayer;
QT_END_NAMESPACE

namespace ibmsc {

class AudioPreviewService : public QObject {
    Q_OBJECT
public:
    explicit AudioPreviewService(QObject* parent = nullptr);
    ~AudioPreviewService() override;

    void play(const QString& absoluteOrRelativePath, const QString& rootDir);
    void stop();

private:
    QString resolvePlayablePath(const QString& absoluteOrRelativePath, const QString& rootDir) const;

    QMediaPlayer* m_player;
    QAudioOutput* m_output;
};

} // namespace ibmsc

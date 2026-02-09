#pragma once

#include <QHash>
#include <QString>
#include <QVector>

namespace ibmsc {

struct BmsNote {
    int columnIndex = 0;
    double vPosition = 0.0;
    double length = 0.0; // NT long-note length; 0 means normal note
    int value = 0; // Internal value uses x10000 scale (for BPM/STOP compatibility)
    bool longNote = false;
    bool hidden = false;
    bool landmine = false;
    bool selected = false;
    QString channelId;
};

struct BmsHeader {
    QString title;
    QString artist;
    QString genre;
    QString subtitle;
    QString subartist;
    QString stageFile;
    QString banner;
    QString backBmp;
    QString playLevel;
    QString total;
    QString comment;
    QString exRank;
    int player = 1;
    int rank = 2;
    int difficulty = 1;
    int lnObj = 0;
    double bpm = 120.0;
};

class BmsDocument {
public:
    static constexpr int kTableSize = 1296;
    static constexpr int kMaxMeasures = 1000;

    BmsDocument();
    void clear();

    void recomputeMeasureBottom();
    int measureAtPosition(double vPosition) const;
    double measureBottomAt(int idx) const;
    double measureLengthAt(int idx) const;

    static int channelToColumn(const QString& channel);
    static QString columnToDefaultChannel(int column);

    QVector<BmsNote> notes;
    BmsHeader header;
    QVector<double> measureLengths;
    QVector<double> measureBottom;

    QVector<QString> wavTable;
    QVector<QString> bmpTable;
    QVector<qint64> bpmTable;
    QVector<qint64> stopTable;
    QVector<qint64> scrollTable;

    QString expansion;
    QString sourcePath;
};

} // namespace ibmsc

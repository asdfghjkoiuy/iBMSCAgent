#include "core/bms_document.h"

#include <QtMath>

namespace ibmsc {

namespace {
const QVector<QString> kChannelList = {
    "01", "03", "04", "06", "07", "08", "09",
    "11", "12", "13", "14", "15", "16", "18", "19",
    "21", "22", "23", "24", "25", "26", "28", "29",
    "31", "32", "33", "34", "35", "36", "38", "39",
    "41", "42", "43", "44", "45", "46", "48", "49",
    "51", "52", "53", "54", "55", "56", "58", "59",
    "61", "62", "63", "64", "65", "66", "68", "69",
    "D1", "D2", "D3", "D4", "D5", "D6", "D8", "D9",
    "E1", "E2", "E3", "E4", "E5", "E6", "E8", "E9",
    "SC"
};

QHash<QString, int> buildChannelMap() {
    QHash<QString, int> map;
    for (int i = 0; i < kChannelList.size(); ++i) {
        map.insert(kChannelList[i], i + 1);
    }
    return map;
}

const QHash<QString, int> kChannelMap = buildChannelMap();
} // namespace

BmsDocument::BmsDocument() {
    clear();
}

void BmsDocument::clear() {
    notes.clear();
    header = BmsHeader{};
    measureLengths = QVector<double>(kMaxMeasures, 192.0);
    measureBottom = QVector<double>(kMaxMeasures, 0.0);
    wavTable = QVector<QString>(kTableSize);
    bmpTable = QVector<QString>(kTableSize);
    bpmTable = QVector<qint64>(kTableSize, 0);
    stopTable = QVector<qint64>(kTableSize, 0);
    scrollTable = QVector<qint64>(kTableSize, 0);
    expansion.clear();
    sourcePath.clear();

    BmsNote bpmNote;
    bpmNote.columnIndex = channelToColumn("03");
    bpmNote.vPosition = -1.0;
    bpmNote.value = static_cast<int>(header.bpm * 10000.0);
    bpmNote.channelId = "03";
    notes.push_back(bpmNote);

    recomputeMeasureBottom();
}

void BmsDocument::recomputeMeasureBottom() {
    if (measureBottom.isEmpty()) {
        return;
    }
    measureBottom[0] = 0.0;
    for (int i = 1; i < measureBottom.size(); ++i) {
        measureBottom[i] = measureBottom[i - 1] + measureLengths[i - 1];
    }
}

int BmsDocument::measureAtPosition(double vPosition) const {
    if (vPosition <= 0.0) {
        return 0;
    }
    for (int i = 0; i < measureBottom.size(); ++i) {
        const double bottom = measureBottom[i];
        const double top = bottom + measureLengths[i];
        if (vPosition >= bottom && vPosition < top) {
            return i;
        }
    }
    return measureBottom.size() - 1;
}

double BmsDocument::measureBottomAt(int idx) const {
    if (idx < 0 || idx >= measureBottom.size()) {
        return 0.0;
    }
    return measureBottom[idx];
}

double BmsDocument::measureLengthAt(int idx) const {
    if (idx < 0 || idx >= measureLengths.size()) {
        return 192.0;
    }
    return measureLengths[idx];
}

int BmsDocument::channelToColumn(const QString& channel) {
    return kChannelMap.value(channel.toUpper(), 0);
}

QString BmsDocument::columnToDefaultChannel(int column) {
    if (column <= 0 || column > kChannelList.size()) {
        return "11";
    }
    return kChannelList[column - 1];
}

} // namespace ibmsc

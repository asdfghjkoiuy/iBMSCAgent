#include "core/bms_document.h"

#include <QtMath>

#include <array>

namespace ibmsc {

namespace {
struct ColumnMeta {
    int index = 0;
    int identifier = 0;
    const char* title = "";
    bool isNoteCol = false;
    bool isNumeric = false;
    bool isSound = false;
    bool isVisible = true;
    int width = 40;
};

enum ColumnIndex : int {
    niMeasure = 0,
    niSCROLL = 1,
    niBPM = 2,
    niSTOP = 3,
    niS1 = 4,
    niA1 = 5,
    niA2 = 6,
    niA3 = 7,
    niA4 = 8,
    niA5 = 9,
    niA6 = 10,
    niA7 = 11,
    niA8 = 12,
    niS2 = 13,
    niD1 = 14,
    niD2 = 15,
    niD3 = 16,
    niD4 = 17,
    niD5 = 18,
    niD6 = 19,
    niD7 = 20,
    niD8 = 21,
    niS3 = 22,
    niBGA = 23,
    niLAYER = 24,
    niPOOR = 25,
    niS4 = 26,
    niB = 27
};

const std::array<ColumnMeta, 28> kColumns = {{
    {niMeasure, 0, "Measure", false, true, false, true, 50},
    {niSCROLL, 99, "SCROLL", true, true, false, true, 60},
    {niBPM, 3, "BPM", true, true, false, true, 60},
    {niSTOP, 9, "STOP", true, true, false, true, 50},
    {niS1, 0, "", false, false, false, true, 5},
    {niA1, 0x16, "A1", true, false, true, true, 42},
    {niA2, 0x11, "A2", true, false, true, true, 30},
    {niA3, 0x12, "A3", true, false, true, true, 42},
    {niA4, 0x13, "A4", true, false, true, true, 45},
    {niA5, 0x14, "A5", true, false, true, true, 42},
    {niA6, 0x15, "A6", true, false, true, true, 30},
    {niA7, 0x18, "A7", true, false, true, true, 42},
    {niA8, 0x19, "A8", true, false, true, true, 0},
    {niS2, 0, "", false, false, false, true, 5},
    {niD1, 0x21, "D1", true, false, true, false, 42},
    {niD2, 0x22, "D2", true, false, true, false, 30},
    {niD3, 0x23, "D3", true, false, true, false, 42},
    {niD4, 0x24, "D4", true, false, true, false, 45},
    {niD5, 0x25, "D5", true, false, true, false, 42},
    {niD6, 0x28, "D6", true, false, true, false, 30},
    {niD7, 0x29, "D7", true, false, true, false, 42},
    {niD8, 0x26, "D8", true, false, true, false, 40},
    {niS3, 0, "", false, false, false, false, 5},
    {niBGA, 4, "BGA", true, false, false, false, 40},
    {niLAYER, 7, "LAYER", true, false, false, false, 40},
    {niPOOR, 6, "POOR", true, false, false, false, 40},
    {niS4, 0, "", false, false, false, false, 5},
    {niB, 1, "B", true, false, true, true, 40},
}};

QString normalizeChannel(QString channel) {
    channel = channel.trimmed().toUpper();
    if (channel.size() == 1) {
        channel.prepend('0');
    }
    return channel;
}

const ColumnMeta* metaForColumn(int column) {
    if (column >= 0 && column < static_cast<int>(kColumns.size())) {
        return &kColumns[static_cast<size_t>(column)];
    }
    if (column >= niB) {
        return &kColumns[niB];
    }
    return nullptr;
}

QString identifierToChannel(int identifier) {
    if (identifier == 99) {
        return "SC";
    }
    if (identifier <= 0) {
        return QString();
    }
    return QString("%1").arg(identifier, 2, 16, QChar('0')).toUpper();
}
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

int BmsDocument::columnCount() {
    return static_cast<int>(kColumns.size());
}

int BmsDocument::channelToColumn(const QString& rawChannel) {
    const QString channel = normalizeChannel(rawChannel);
    if (channel.size() == 3) {
        bool ok = false;
        const int ext = channel.toInt(&ok);
        if (ok && ext >= 101) {
            return niB + (ext - 101);
        }
    }
    if (channel == "01") return niB;
    if (channel == "03" || channel == "08") return niBPM;
    if (channel == "09") return niSTOP;
    if (channel == "SC") return niSCROLL;
    if (channel == "04") return niBGA;
    if (channel == "07") return niLAYER;
    if (channel == "06") return niPOOR;

    if (channel == "16" || channel == "36" || channel == "56" || channel == "76" || channel == "D6") return niA1;
    if (channel == "11" || channel == "31" || channel == "51" || channel == "71" || channel == "D1") return niA2;
    if (channel == "12" || channel == "32" || channel == "52" || channel == "72" || channel == "D2") return niA3;
    if (channel == "13" || channel == "33" || channel == "53" || channel == "73" || channel == "D3") return niA4;
    if (channel == "14" || channel == "34" || channel == "54" || channel == "74" || channel == "D4") return niA5;
    if (channel == "15" || channel == "35" || channel == "55" || channel == "75" || channel == "D5") return niA6;
    if (channel == "18" || channel == "38" || channel == "58" || channel == "78" || channel == "D8") return niA7;
    if (channel == "19" || channel == "39" || channel == "59" || channel == "79" || channel == "D9") return niA8;

    if (channel == "21" || channel == "41" || channel == "61" || channel == "81" || channel == "E1") return niD1;
    if (channel == "22" || channel == "42" || channel == "62" || channel == "82" || channel == "E2") return niD2;
    if (channel == "23" || channel == "43" || channel == "63" || channel == "83" || channel == "E3") return niD3;
    if (channel == "24" || channel == "44" || channel == "64" || channel == "84" || channel == "E4") return niD4;
    if (channel == "25" || channel == "45" || channel == "65" || channel == "85" || channel == "E5") return niD5;
    if (channel == "28" || channel == "48" || channel == "68" || channel == "88" || channel == "E8") return niD6;
    if (channel == "29" || channel == "49" || channel == "69" || channel == "89" || channel == "E9") return niD7;
    if (channel == "26" || channel == "46" || channel == "66" || channel == "86" || channel == "E6") return niD8;

    return 0;
}

QString BmsDocument::columnToDefaultChannel(int column) {
    const ColumnMeta* meta = metaForColumn(column);
    if (!meta) {
        return "11";
    }
    if (column >= niB) {
        return QString("%1").arg(101 + (column - niB), 3, 10, QChar('0'));
    }
    const QString ch = identifierToChannel(meta->identifier);
    return ch.isEmpty() ? "11" : ch;
}

QString BmsDocument::columnTitle(int column) {
    const ColumnMeta* meta = metaForColumn(column);
    if (!meta) {
        return QString("C%1").arg(column);
    }
    if (column >= niB) {
        return QString("B%1").arg(column - niB + 1);
    }
    return QString::fromUtf8(meta->title);
}

int BmsDocument::columnIdentifier(int column) {
    const ColumnMeta* meta = metaForColumn(column);
    return meta ? meta->identifier : 0;
}

bool BmsDocument::isSoundColumn(int column) {
    const ColumnMeta* meta = metaForColumn(column);
    return meta && meta->isSound;
}

bool BmsDocument::isNumericColumn(int column) {
    const ColumnMeta* meta = metaForColumn(column);
    return meta && meta->isNumeric;
}

bool BmsDocument::isVisibleColumn(int column) {
    const ColumnMeta* meta = metaForColumn(column);
    return meta && meta->isVisible;
}

bool BmsDocument::isEnabledColumn(int column) {
    const ColumnMeta* meta = metaForColumn(column);
    return meta && meta->isVisible && meta->isNoteCol && meta->width > 0;
}

} // namespace ibmsc

#include "io/bms_parser.h"

#include "core/bms_document.h"

#include <QFile>
#include <QMap>
#include <QRegularExpression>
#include <QStringList>
#include <QTextStream>

#include <algorithm>
#include <cmath>

namespace ibmsc {

namespace {
int c36To10(const QString& raw) {
    const QString value = raw.trimmed().toUpper();
    int out = 0;
    for (QChar c : value) {
        int v = 0;
        if (c >= '0' && c <= '9') {
            v = c.unicode() - '0';
        } else if (c >= 'A' && c <= 'Z') {
            v = c.unicode() - 'A' + 10;
        } else {
            return 0;
        }
        out = out * 36 + v;
    }
    return out;
}

QString c10To36(int value, int width = 2) {
    static const char* kDigits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (value <= 0) {
        return QString(width, '0');
    }
    QString out;
    int v = value;
    while (v > 0) {
        out.prepend(QChar(kDigits[v % 36]));
        v /= 36;
    }
    while (out.size() < width) {
        out.prepend('0');
    }
    return out.right(width);
}

bool isLongChannel(const QString& ch) {
    const int v = c36To10(ch);
    return v >= 50 && v < 90;
}

QString toLongChannel(const QString& channel) {
    QString ch = channel.trimmed().toUpper();
    if (ch.size() == 1) {
        ch.prepend('0');
    }
    if (ch.size() != 2) {
        return ch;
    }
    if (ch[0] == '1') return QString("5") + ch[1];
    if (ch[0] == '2') return QString("6") + ch[1];
    if (ch[0] == '3') return QString("7") + ch[1];
    if (ch[0] == '4') return QString("8") + ch[1];
    return ch;
}

bool isHiddenChannel(const QString& ch) {
    const int v = c36To10(ch);
    return (v >= 30 && v < 50) || (v >= 70 && v < 90);
}

bool isLandmineChannel(const QString& ch) {
    const int start = c36To10("D0");
    const int end = c36To10("EZ");
    const int v = c36To10(ch);
    return v > start && v < end;
}

QString decodeText(const QByteArray& bytes) {
    QString utf8 = QString::fromUtf8(bytes);
    if (!utf8.contains(QChar(0xFFFD))) {
        return utf8;
    }
    return QString::fromLocal8Bit(bytes);
}

int ensureValueIndex(QVector<qint64>& table, qint64 value) {
    if (value <= 0) {
        return 0;
    }
    for (int i = 1; i < table.size(); ++i) {
        if (table[i] == value) {
            return i;
        }
    }
    for (int i = 1; i < table.size(); ++i) {
        if (table[i] == 0) {
            table[i] = value;
            return i;
        }
    }
    return 0;
}

QString normalizeChannel(const QString& channel) {
    QString ch = channel.trimmed().toUpper();
    if (ch.size() == 1) {
        ch.prepend('0');
    }
    return ch;
}

QString sanitizeResourcePath(const QString& raw) {
    QString out = raw.trimmed();
    bool inSingle = false;
    bool inDouble = false;
    int cut = -1;
    for (int i = 0; i < out.size(); ++i) {
        const QChar c = out[i];
        if (c == '\'' && !inDouble) {
            inSingle = !inSingle;
            continue;
        }
        if (c == '"' && !inSingle) {
            inDouble = !inDouble;
            continue;
        }
        if (c == ';' && !inSingle && !inDouble) {
            cut = i;
            break;
        }
    }
    if (cut >= 0) {
        out = out.left(cut).trimmed();
    }
    if ((out.startsWith('"') && out.endsWith('"')) || (out.startsWith('\'') && out.endsWith('\''))) {
        if (out.size() >= 2) {
            out = out.mid(1, out.size() - 2);
        }
    }
    return out.replace('\\', '/').trimmed();
}

bool parseIndexedDirective(const QString& line, int prefixLen, int* outIndex, QString* outPayload) {
    if (!outIndex) {
        return false;
    }
    const QString tail = line.mid(prefixLen).trimmed();
    if (tail.size() < 2) {
        return false;
    }
    const int idx = c36To10(tail.left(2));
    if (idx <= 0) {
        return false;
    }
    *outIndex = idx;
    if (outPayload) {
        *outPayload = tail.mid(2).trimmed();
    }
    return true;
}

} // namespace

bool BmsParser::loadFromFile(const QString& filePath, BmsDocument& outDoc, QString* error) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        if (error) {
            *error = QString("Cannot open file: %1").arg(f.errorString());
        }
        return false;
    }

    const QString text = decodeText(f.readAll())
                             .replace("\r\n", "\n")
                             .replace('\r', '\n');
    const QStringList lines = text.split('\n', Qt::KeepEmptyParts);

    outDoc.clear();
    outDoc.sourcePath = filePath;

    QRegularExpression noteLineRe("^#(\\d{3})([0-9A-Za-z]{2,3}):(.*)$");

    int stackDepth = 0;
    for (const QString& rawLine : lines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (stackDepth > 0) {
            if (line.startsWith("#IF", Qt::CaseInsensitive) ||
                line.startsWith("#SWITCH", Qt::CaseInsensitive) ||
                line.startsWith("#SETSWITCH", Qt::CaseInsensitive)) {
                ++stackDepth;
            } else if (line.startsWith("#ENDIF", Qt::CaseInsensitive) ||
                       line.startsWith("#ENDSW", Qt::CaseInsensitive)) {
                --stackDepth;
            }
            outDoc.expansion += rawLine + "\n";
            continue;
        }

        if (line.startsWith("#") && line.mid(4, 3) == "02:") {
            const int measure = line.mid(1, 3).toInt();
            const double ratio = line.mid(7).toDouble();
            if (measure >= 0 && measure < outDoc.measureLengths.size() && ratio > 0.0) {
                outDoc.measureLengths[measure] = ratio * 192.0;
            }
            continue;
        }

        if (line.startsWith("#WAV", Qt::CaseInsensitive)) {
            int idx = 0;
            QString payload;
            if (parseIndexedDirective(line, 4, &idx, &payload) && idx < outDoc.wavTable.size()) {
                outDoc.wavTable[idx] = sanitizeResourcePath(payload);
            }
            continue;
        }
        if (line.startsWith("#BMP", Qt::CaseInsensitive)) {
            int idx = 0;
            QString payload;
            if (parseIndexedDirective(line, 4, &idx, &payload) && idx < outDoc.bmpTable.size()) {
                outDoc.bmpTable[idx] = sanitizeResourcePath(payload);
            }
            continue;
        }
        if (line.startsWith("#BPM", Qt::CaseInsensitive) && !line.mid(4, 1).trimmed().isEmpty()) {
            int idx = 0;
            QString payload;
            if (parseIndexedDirective(line, 4, &idx, &payload) && idx < outDoc.bpmTable.size()) {
                outDoc.bpmTable[idx] = static_cast<qint64>(payload.toDouble() * 10000.0);
            }
            continue;
        }
        if (line.startsWith("#STOP", Qt::CaseInsensitive)) {
            int idx = 0;
            QString payload;
            if (parseIndexedDirective(line, 5, &idx, &payload) && idx < outDoc.stopTable.size()) {
                outDoc.stopTable[idx] = static_cast<qint64>(payload.toDouble() * 10000.0);
            }
            continue;
        }
        if (line.startsWith("#SCROLL", Qt::CaseInsensitive)) {
            int idx = 0;
            QString payload;
            if (parseIndexedDirective(line, 7, &idx, &payload) && idx < outDoc.scrollTable.size()) {
                outDoc.scrollTable[idx] = static_cast<qint64>(payload.toDouble() * 10000.0);
            }
            continue;
        }

        if (line.startsWith("#TITLE", Qt::CaseInsensitive)) outDoc.header.title = line.mid(6).trimmed();
        else if (line.startsWith("#ARTIST", Qt::CaseInsensitive)) outDoc.header.artist = line.mid(7).trimmed();
        else if (line.startsWith("#GENRE", Qt::CaseInsensitive)) outDoc.header.genre = line.mid(6).trimmed();
        else if (line.startsWith("#SUBTITLE", Qt::CaseInsensitive)) outDoc.header.subtitle = line.mid(9).trimmed();
        else if (line.startsWith("#SUBARTIST", Qt::CaseInsensitive)) outDoc.header.subartist = line.mid(10).trimmed();
        else if (line.startsWith("#STAGEFILE", Qt::CaseInsensitive)) outDoc.header.stageFile = line.mid(10).trimmed();
        else if (line.startsWith("#BANNER", Qt::CaseInsensitive)) outDoc.header.banner = line.mid(7).trimmed();
        else if (line.startsWith("#BACKBMP", Qt::CaseInsensitive)) outDoc.header.backBmp = line.mid(8).trimmed();
        else if (line.startsWith("#PLAYLEVEL", Qt::CaseInsensitive)) outDoc.header.playLevel = line.mid(10).trimmed();
        else if (line.startsWith("#TOTAL", Qt::CaseInsensitive)) outDoc.header.total = line.mid(6).trimmed();
        else if (line.startsWith("#COMMENT", Qt::CaseInsensitive)) outDoc.header.comment = line.mid(8).trimmed();
        else if (line.startsWith("#DEFEXRANK", Qt::CaseInsensitive)) outDoc.header.exRank = line.mid(10).trimmed();
        else if (line.startsWith("#PLAYER", Qt::CaseInsensitive)) outDoc.header.player = std::max(1, line.mid(7).trimmed().toInt());
        else if (line.startsWith("#RANK", Qt::CaseInsensitive)) outDoc.header.rank = std::max(0, line.mid(5).trimmed().toInt());
        else if (line.startsWith("#DIFFICULTY", Qt::CaseInsensitive)) outDoc.header.difficulty = std::max(0, line.mid(11).trimmed().toInt());
        else if (line.startsWith("#LNOBJ", Qt::CaseInsensitive)) outDoc.header.lnObj = c36To10(line.mid(6).trimmed());
        else if (line.startsWith("#BPM", Qt::CaseInsensitive)) outDoc.header.bpm = line.mid(4).trimmed().toDouble();

        QRegularExpressionMatch m = noteLineRe.match(line);
        if (m.hasMatch()) {
            const QString channel = normalizeChannel(m.captured(2));
            if (BmsDocument::channelToColumn(channel) == 0) {
                outDoc.expansion += rawLine + "\n";
            }
            continue;
        }

        if (line.startsWith("#IF", Qt::CaseInsensitive) ||
            line.startsWith("#SWITCH", Qt::CaseInsensitive) ||
            line.startsWith("#SETSWITCH", Qt::CaseInsensitive)) {
            stackDepth = 1;
            outDoc.expansion += rawLine + "\n";
            continue;
        }

        if (line.startsWith('#')) {
            outDoc.expansion += rawLine + "\n";
        }
    }

    outDoc.recomputeMeasureBottom();

    for (const QString& rawLine : lines) {
        const QString line = rawLine.trimmed();
        QRegularExpressionMatch m = noteLineRe.match(line);
        if (!m.hasMatch()) {
            continue;
        }

        const int measure = m.captured(1).toInt();
        const QString channel = normalizeChannel(m.captured(2));
        const QString data = m.captured(3).trimmed();
        if (data.size() < 2 || (data.size() % 2) != 0) {
            continue;
        }

        int column = BmsDocument::channelToColumn(channel);
        if (column == 0) {
            continue;
        }

        const int noteSlots = data.size() / 2;
        for (int i = 0; i < noteSlots; ++i) {
            const QString token = data.mid(i * 2, 2).toUpper();
            if (token == "00") {
                continue;
            }

            BmsNote note;
            note.columnIndex = column;
            note.channelId = channel;
            note.longNote = isLongChannel(channel);
            note.hidden = isHiddenChannel(channel);
            note.landmine = isLandmineChannel(channel);
            note.vPosition = outDoc.measureBottomAt(measure) +
                             outDoc.measureLengthAt(measure) * (static_cast<double>(i) / noteSlots);

            if (channel == "03") {
                note.value = token.toInt(nullptr, 16) * 10000;
            } else if (channel == "08") {
                note.value = static_cast<int>(outDoc.bpmTable[c36To10(token)]);
            } else if (channel == "09") {
                note.value = static_cast<int>(outDoc.stopTable[c36To10(token)]);
            } else if (channel == "SC") {
                note.value = static_cast<int>(outDoc.scrollTable[c36To10(token)]);
            } else {
                note.value = c36To10(token) * 10000;
            }
            outDoc.notes.push_back(note);
        }
    }

    std::sort(outDoc.notes.begin(), outDoc.notes.end(), [](const BmsNote& a, const BmsNote& b) {
        if (a.vPosition == b.vPosition) {
            return a.columnIndex < b.columnIndex;
        }
        return a.vPosition < b.vPosition;
    });

    // Reconstruct NT-style long note length from long channels by pairing same column/value events.
    QHash<QString, int> lnOpen;
    for (int i = 0; i < outDoc.notes.size(); ++i) {
        BmsNote& n = outDoc.notes[i];
        if (!n.longNote || n.vPosition < 0.0) {
            continue;
        }
        const QString key = QString("%1|%2|%3").arg(n.columnIndex).arg(n.value).arg(n.channelId);
        if (lnOpen.contains(key)) {
            const int startIdx = lnOpen.take(key);
            if (startIdx >= 0 && startIdx < outDoc.notes.size()) {
                BmsNote& s = outDoc.notes[startIdx];
                if (n.vPosition > s.vPosition) {
                    s.length = n.vPosition - s.vPosition;
                }
                n.vPosition = -1.0; // hide paired end marker in editor model
            }
        } else {
            lnOpen.insert(key, i);
        }
    }

    return true;
}

bool BmsParser::saveToFile(const QString& filePath, BmsDocument& doc, QString* error) {
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error) {
            *error = QString("Cannot save file: %1").arg(f.errorString());
        }
        return false;
    }

    doc.recomputeMeasureBottom();

    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);

    out << "#TITLE " << doc.header.title << "\n";
    out << "#ARTIST " << doc.header.artist << "\n";
    out << "#GENRE " << doc.header.genre << "\n";
    out << "#SUBTITLE " << doc.header.subtitle << "\n";
    out << "#SUBARTIST " << doc.header.subartist << "\n";
    out << "#STAGEFILE " << doc.header.stageFile << "\n";
    out << "#BANNER " << doc.header.banner << "\n";
    out << "#BACKBMP " << doc.header.backBmp << "\n";
    out << "#PLAYER " << doc.header.player << "\n";
    out << "#RANK " << doc.header.rank << "\n";
    out << "#PLAYLEVEL " << doc.header.playLevel << "\n";
    out << "#DIFFICULTY " << doc.header.difficulty << "\n";
    out << "#TOTAL " << doc.header.total << "\n";
    out << "#DEFEXRANK " << doc.header.exRank << "\n";
    out << "#COMMENT " << doc.header.comment << "\n";
    out << "#BPM " << QString::number(doc.header.bpm, 'f', 6) << "\n";
    if (doc.header.lnObj > 0) {
        out << "#LNOBJ " << c10To36(doc.header.lnObj) << "\n";
    }

    for (int i = 0; i < doc.measureLengths.size(); ++i) {
        const double ratio = doc.measureLengthAt(i) / 192.0;
        if (std::abs(ratio - 1.0) > 0.000001) {
            out << '#' << QString("%1").arg(i, 3, 10, QChar('0')) << "02:" << QString::number(ratio, 'f', 10) << "\n";
        }
    }

    for (int i = 1; i < doc.wavTable.size(); ++i) {
        if (!doc.wavTable[i].trimmed().isEmpty()) {
            out << "#WAV" << c10To36(i) << doc.wavTable[i] << "\n";
        }
    }
    for (int i = 1; i < doc.bmpTable.size(); ++i) {
        if (!doc.bmpTable[i].trimmed().isEmpty()) {
            out << "#BMP" << c10To36(i) << doc.bmpTable[i] << "\n";
        }
    }
    for (int i = 1; i < doc.bpmTable.size(); ++i) {
        if (doc.bpmTable[i] != 0) {
            out << "#BPM" << c10To36(i) << QString::number(doc.bpmTable[i] / 10000.0, 'f', 6) << "\n";
        }
    }
    for (int i = 1; i < doc.stopTable.size(); ++i) {
        if (doc.stopTable[i] != 0) {
            out << "#STOP" << c10To36(i) << QString::number(doc.stopTable[i] / 10000.0, 'f', 6) << "\n";
        }
    }
    for (int i = 1; i < doc.scrollTable.size(); ++i) {
        if (doc.scrollTable[i] != 0) {
            out << "#SCROLL" << c10To36(i) << QString::number(doc.scrollTable[i] / 10000.0, 'f', 6) << "\n";
        }
    }

    struct ChannelGrid {
        int measure = 0;
        QString channel;
        QString data;
    };

    QMap<QString, QVector<QString>> map;

    struct SaveEvent {
        int measure = 0;
        QString channel;
        int slot = 0;
        int value = 0;
    };
    QVector<SaveEvent> events;
    events.reserve(doc.notes.size() * 2);

    for (const BmsNote& n : doc.notes) {
        if (n.vPosition < 0) {
            continue;
        }

        const int measure = doc.measureAtPosition(n.vPosition);
        QString channel = normalizeChannel(n.channelId.isEmpty() ? BmsDocument::columnToDefaultChannel(n.columnIndex) : n.channelId);
        if ((n.longNote || n.length > 0.0) && !isLongChannel(channel)) {
            channel = toLongChannel(channel);
        }

        const double rel = (n.vPosition - doc.measureBottomAt(measure)) / doc.measureLengthAt(measure);
        const int slot = std::clamp(static_cast<int>(std::round(rel * 191.0)), 0, 191);
        events.push_back({measure, channel, slot, n.value});

        if (n.length > 0.0) {
            const double endPos = n.vPosition + n.length;
            const int eMeasure = doc.measureAtPosition(endPos);
            const double eRel = (endPos - doc.measureBottomAt(eMeasure)) / doc.measureLengthAt(eMeasure);
            const int eSlot = std::clamp(static_cast<int>(std::round(eRel * 191.0)), 0, 191);
            events.push_back({eMeasure, channel, eSlot, n.value});
        }
    }

    for (const SaveEvent& ev : events) {
        const QString key = QString("%1:%2").arg(ev.measure).arg(ev.channel);
        QVector<QString>& grid = map[key];
        if (grid.isEmpty()) {
            grid = QVector<QString>(192, "00");
        }

        QString token = "00";
        if (ev.channel == "03") {
            token = QString("%1").arg((ev.value / 10000) & 0xFF, 2, 16, QChar('0')).toUpper();
        } else if (ev.channel == "08") {
            const int idx = ensureValueIndex(doc.bpmTable, ev.value);
            token = c10To36(idx);
        } else if (ev.channel == "09") {
            const int idx = ensureValueIndex(doc.stopTable, ev.value);
            token = c10To36(idx);
        } else if (ev.channel == "SC") {
            const int idx = ensureValueIndex(doc.scrollTable, ev.value);
            token = c10To36(idx);
        } else {
            token = c10To36(std::max(0, ev.value / 10000));
        }

        grid[ev.slot] = token;
    }

    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        const QStringList parts = it.key().split(':');
        const int measure = parts[0].toInt();
        const QString channel = parts[1];
        QString data;
        for (const QString& t : it.value()) {
            data += t;
        }
        while (data.endsWith("00") && data.size() > 2) {
            data.chop(2);
        }
        out << '#' << QString("%1").arg(measure, 3, 10, QChar('0')) << channel << ':' << data << "\n";
    }

    if (!doc.expansion.trimmed().isEmpty()) {
        out << "\n" << doc.expansion.trimmed() << "\n";
    }

    return true;
}

} // namespace ibmsc

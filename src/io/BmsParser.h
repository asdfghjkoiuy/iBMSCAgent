#pragma once
#include "model/BmsDocument.h"
#include <QString>
#include <QStringList>
#include <vector>

class QFile;

namespace IO {

enum class ParseWarningCode {
    MissingBpm,
    DuplicateChannelRow,
    InvalidBase36Token,
    OutOfRangeSlot,
    UnknownDirective,
};

struct ParseWarning {
    ParseWarningCode code;
    int line = 0;
    QString message;
};

struct ParseResult {
    Model::BmsDocument doc;
    std::vector<ParseWarning> warnings;
};

class BmsParser {
public:
    /// Parses a BMS/BME/BML/PMS file. The file must be open and readable.
    static ParseResult parse(QFile& file);

private:
    static void processLine(const QString& line, int lineNum, ParseResult& result,
                            std::vector<std::pair<int,int>>& seenChannelRows);
    static void decodeChannelRow(int measure, int channel, const QString& data,
                                 Model::BmsDocument& doc, std::vector<ParseWarning>& warnings,
                                 int lineNum);
};

} // namespace IO

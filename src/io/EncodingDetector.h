#pragma once
#include <QByteArray>
#include <QString>

namespace IO {

enum class Encoding {
    UTF8,
    ShiftJIS,
    EucJP,
    Unknown
};

/// Detects the text encoding of a raw BMS file byte array.
/// Checks for UTF-8 BOM first, then uses heuristics.
Encoding detectEncoding(const QByteArray& data);

/// Decodes raw bytes to a QString using the detected (or specified) encoding.
QString decodeBytes(const QByteArray& data, Encoding enc);

/// Returns the QStringConverter::Encoding for the given Encoding.
const char* encodingToCodecName(Encoding enc); // returns IANA name for reference

} // namespace IO

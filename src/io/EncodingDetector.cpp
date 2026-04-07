#include "io/EncodingDetector.h"
#include <QStringConverter>

namespace IO {

Encoding detectEncoding(const QByteArray& data) {
    // UTF-8 BOM: EF BB BF
    if (data.size() >= 3 &&
        (unsigned char)data[0] == 0xEF &&
        (unsigned char)data[1] == 0xBB &&
        (unsigned char)data[2] == 0xBF) {
        return Encoding::UTF8;
    }

    // Heuristic: scan for Shift-JIS or EUC-JP byte patterns
    int shiftJisScore = 0;
    int eucJpScore = 0;
    int validUtf8 = 0;
    int invalidUtf8 = 0;

    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(data.constData());
    int len = data.size();

    for (int i = 0; i < len; ) {
        unsigned char b = bytes[i];

        // Shift-JIS lead bytes: 0x81-0x9F, 0xE0-0xEF
        if ((b >= 0x81 && b <= 0x9F) || (b >= 0xE0 && b <= 0xEF)) {
            if (i + 1 < len) {
                unsigned char b2 = bytes[i + 1];
                if ((b2 >= 0x40 && b2 <= 0x7E) || (b2 >= 0x80 && b2 <= 0xFC)) {
                    shiftJisScore += 2;
                    i += 2;
                    continue;
                }
            }
        }
        // EUC-JP lead bytes: 0xA1-0xFE
        if (b >= 0xA1 && b <= 0xFE) {
            if (i + 1 < len) {
                unsigned char b2 = bytes[i + 1];
                if (b2 >= 0xA1 && b2 <= 0xFE) {
                    eucJpScore += 2;
                    i += 2;
                    continue;
                }
            }
        }
        // Check for valid UTF-8 multi-byte sequences
        if (b >= 0xC0 && b < 0xF8) {
            int seq = (b < 0xE0) ? 2 : (b < 0xF0) ? 3 : 4;
            bool ok = (i + seq <= len);
            for (int j = 1; j < seq && ok; ++j)
                if ((bytes[i + j] & 0xC0) != 0x80) ok = false;
            if (ok) { validUtf8 += seq; i += seq; continue; }
            else { invalidUtf8++; }
        }
        ++i;
    }

    if (invalidUtf8 == 0 && validUtf8 > 0) return Encoding::UTF8;
    if (shiftJisScore > eucJpScore) return Encoding::ShiftJIS;
    if (eucJpScore > 0) return Encoding::EucJP;
    return Encoding::UTF8; // default: treat as UTF-8
}

const char* encodingToCodecName(Encoding enc) {
    switch (enc) {
        case Encoding::UTF8:    return "UTF-8";
        case Encoding::ShiftJIS: return "Shift-JIS";
        case Encoding::EucJP:   return "EUC-JP";
        default:                return "UTF-8";
    }
}

QString decodeBytes(const QByteArray& data, Encoding enc) {
    // Skip UTF-8 BOM if present
    QByteArray payload = data;
    if (enc == Encoding::UTF8 && data.size() >= 3 &&
        (unsigned char)data[0] == 0xEF &&
        (unsigned char)data[1] == 0xBB &&
        (unsigned char)data[2] == 0xBF) {
        payload = data.mid(3);
    }

    QStringConverter::Encoding qtEnc = QStringConverter::Utf8;
    if (enc == Encoding::ShiftJIS) qtEnc = QStringConverter::System; // fallback; real SJIS via ICU
    // Qt 6.4+ supports "Shift_JIS" via QStringConverter if ICU is linked
    // For now use Utf8 as safe fallback for non-SJIS content
    if (enc == Encoding::ShiftJIS || enc == Encoding::EucJP) {
        // Try to create a decoder from IANA name
        auto optEnc = QStringConverter::encodingForName(encodingToCodecName(enc));
        if (optEnc.has_value()) qtEnc = optEnc.value();
    }

    QStringDecoder decoder(qtEnc);
    return decoder(payload);
}

} // namespace IO

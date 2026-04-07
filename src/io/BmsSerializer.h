#pragma once
#include "model/BmsDocument.h"
#include "io/EncodingDetector.h"

class QFile;

namespace IO {

class BmsSerializer {
public:
    /// Serializes a BmsDocument to a BMS text file.
    /// @param doc        The document to serialize.
    /// @param file       Open writable QFile to write to.
    /// @param encoding   Output encoding (UTF8 or ShiftJIS).
    static bool save(const Model::BmsDocument& doc, QFile& file, Encoding encoding = Encoding::UTF8);
};

} // namespace IO

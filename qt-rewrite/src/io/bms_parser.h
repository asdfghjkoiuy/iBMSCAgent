#pragma once

#include <QString>

namespace ibmsc {

class BmsDocument;

class BmsParser {
public:
    static bool loadFromFile(const QString& filePath, BmsDocument& outDoc, QString* error);
    static bool saveToFile(const QString& filePath, BmsDocument& doc, QString* error);
};

} // namespace ibmsc

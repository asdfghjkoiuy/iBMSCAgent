#include "model/BmsDocument.h"
#include <cstdlib>

namespace Model {

double BmsDocument::measureLength(int measure) const {
    auto it = measureLengths.find(measure);
    return it != measureLengths.end() ? it->second : 1.0;
}

std::string BmsDocument::header(const std::string& key, const std::string& defaultVal) const {
    auto it = headers.find(key);
    return it != headers.end() ? it->second : defaultVal;
}

double BmsDocument::initialBpm() const {
    auto it = headers.find("BPM");
    if (it == headers.end()) return 130.0;
    double v = std::atof(it->second.c_str());
    return v > 0.0 ? v : 130.0;
}

} // namespace Model

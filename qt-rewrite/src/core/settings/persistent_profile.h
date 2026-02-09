#pragma once

#include <QStringList>

namespace ibmsc {

struct PersistentProfile {
    int snap = 16;
    bool ntInput = false;
    int mode = 0;
    bool autoSave = true;
    bool previewOnClick = false;
    bool errorCheck = true;
    QStringList recentFiles;
};

class PersistentProfileStore {
public:
    static PersistentProfile load();
    static void save(const PersistentProfile& profile);
};

} // namespace ibmsc

#include "core/settings/persistent_profile.h"

#include <QSettings>

namespace ibmsc {

PersistentProfile PersistentProfileStore::load() {
    QSettings settings("iBMSCQtRewrite", "iBMSCQt");
    PersistentProfile profile;
    profile.snap = settings.value("editor/snap", 16).toInt();
    profile.ntInput = settings.value("editor/nt_input", false).toBool();
    profile.mode = settings.value("editor/mode", 0).toInt();
    profile.autoSave = settings.value("editor/autosave", true).toBool();
    profile.previewOnClick = settings.value("editor/preview_on_click", false).toBool();
    profile.errorCheck = settings.value("editor/error_check", true).toBool();
    profile.recentFiles = settings.value("file/recent").toStringList();
    return profile;
}

void PersistentProfileStore::save(const PersistentProfile& profile) {
    QSettings settings("iBMSCQtRewrite", "iBMSCQt");
    settings.setValue("editor/snap", profile.snap);
    settings.setValue("editor/nt_input", profile.ntInput);
    settings.setValue("editor/mode", profile.mode);
    settings.setValue("editor/autosave", profile.autoSave);
    settings.setValue("editor/preview_on_click", profile.previewOnClick);
    settings.setValue("editor/error_check", profile.errorCheck);
    settings.setValue("file/recent", profile.recentFiles);
}

} // namespace ibmsc

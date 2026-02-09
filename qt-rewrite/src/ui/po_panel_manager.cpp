#include "ui/po_panel_manager.h"

namespace ibmsc {

void PoPanelManager::registerSection(const Section& section) {
    if (section.key.trimmed().isEmpty()) {
        return;
    }
    m_sections.insert(section.key, section);
}

void PoPanelManager::applySwitchState(const QString& key, bool checked) {
    if (!m_sections.contains(key)) {
        return;
    }
    const Section section = m_sections.value(key);
    if (section.inner) {
        section.inner->setVisible(checked);
    }
}

void PoPanelManager::applyExpandState(const QString& key, bool expanded) {
    if (!m_sections.contains(key)) {
        return;
    }
    const Section section = m_sections.value(key);
    if (section.detail) {
        section.detail->setVisible(expanded);
    }
}

} // namespace ibmsc

#include "ui/po_panel_manager.h"

#include <QCheckBox>

#include <algorithm>

namespace ibmsc {

namespace {
void applyMinHeight(const PoPanelManager::Section& section, bool expanded) {
    if (!section.container) {
        return;
    }
    const int floor = std::max(0, section.minVisibleHeight);
    section.container->setMinimumHeight(expanded ? floor : 0);
}
} // namespace

void PoPanelManager::registerSection(const Section& section) {
    if (section.key.trimmed().isEmpty()) {
        return;
    }
    m_sections.insert(section.key, section);
    const bool expanded = section.switchBox ? section.switchBox->isChecked() : true;
    applyMinHeight(section, expanded);
}

void PoPanelManager::applySwitchState(const QString& key, bool checked) {
    if (!m_sections.contains(key)) {
        return;
    }
    const Section section = m_sections.value(key);
    if (section.inner) {
        section.inner->setVisible(checked);
    }
    applyMinHeight(section, checked);
}

void PoPanelManager::applyExpandState(const QString& key, bool expanded) {
    if (!m_sections.contains(key)) {
        return;
    }
    const Section section = m_sections.value(key);
    if (section.detail) {
        section.detail->setVisible(expanded);
    }
    const bool switchedOn = section.switchBox ? section.switchBox->isChecked() : true;
    applyMinHeight(section, switchedOn && expanded);
}

} // namespace ibmsc

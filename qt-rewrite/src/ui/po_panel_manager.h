#pragma once

#include <QHash>
#include <QString>
#include <QWidget>

class QCheckBox;
class QSplitter;

namespace ibmsc {

class PoPanelManager {
public:
    struct Section {
        QString key;
        QWidget* container = nullptr;
        QWidget* inner = nullptr;
        QWidget* detail = nullptr;
        QCheckBox* switchBox = nullptr;
        QCheckBox* expanderBox = nullptr;
        int minVisibleHeight = 0;
    };

    void registerSection(const Section& section);
    void applySwitchState(const QString& key, bool checked);
    void applyExpandState(const QString& key, bool expanded);

private:
    QHash<QString, Section> m_sections;
};

} // namespace ibmsc

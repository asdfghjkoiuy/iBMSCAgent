#pragma once

#include <QDialog>

namespace ibmsc {

class StatisticsDialog : public QDialog {
    Q_OBJECT
public:
    explicit StatisticsDialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

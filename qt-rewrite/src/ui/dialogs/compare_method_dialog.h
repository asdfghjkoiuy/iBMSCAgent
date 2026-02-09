#pragma once

#include <QDialog>

namespace ibmsc {

class CompareMethodDialog : public QDialog {
    Q_OBJECT
public:
    explicit CompareMethodDialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

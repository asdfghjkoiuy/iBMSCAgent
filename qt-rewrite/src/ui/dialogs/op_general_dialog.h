#pragma once

#include <QDialog>

namespace ibmsc {

class OpGeneralDialog : public QDialog {
    Q_OBJECT
public:
    explicit OpGeneralDialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

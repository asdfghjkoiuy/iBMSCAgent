#pragma once

#include <QDialog>

namespace ibmsc {

class OpPlayerDialog : public QDialog {
    Q_OBJECT
public:
    explicit OpPlayerDialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

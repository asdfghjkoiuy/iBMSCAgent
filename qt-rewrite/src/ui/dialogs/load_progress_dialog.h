#pragma once

#include <QDialog>

namespace ibmsc {

class LoadProgressDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoadProgressDialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

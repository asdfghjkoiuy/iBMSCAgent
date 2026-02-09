#pragma once

#include <QDialog>

namespace ibmsc {

class MyO2Dialog : public QDialog {
    Q_OBJECT
public:
    explicit MyO2Dialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

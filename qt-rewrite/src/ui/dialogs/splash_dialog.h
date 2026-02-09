#pragma once

#include <QDialog>

namespace ibmsc {

class SplashDialog : public QDialog {
    Q_OBJECT
public:
    explicit SplashDialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

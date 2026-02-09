#pragma once

#include <QDialog>

namespace ibmsc {

class AboutDialog : public QDialog {
    Q_OBJECT
public:
    explicit AboutDialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

#pragma once

#include <QDialog>

namespace ibmsc {

class ImportSmDialog : public QDialog {
    Q_OBJECT
public:
    explicit ImportSmDialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

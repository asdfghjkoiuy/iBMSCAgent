#pragma once

#include <QDialog>

namespace ibmsc {

class ColorPickerDialog : public QDialog {
    Q_OBJECT
public:
    explicit ColorPickerDialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

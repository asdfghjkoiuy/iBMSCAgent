#include "ui/dialogs/color_picker_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

ColorPickerDialog::ColorPickerDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Color Picker");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("ColorPickerDialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

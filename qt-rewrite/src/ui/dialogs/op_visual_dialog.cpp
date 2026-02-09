#include "ui/dialogs/op_visual_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

OpVisualDialog::OpVisualDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Visual Settings");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("OpVisualDialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

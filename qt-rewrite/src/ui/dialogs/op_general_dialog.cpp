#include "ui/dialogs/op_general_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

OpGeneralDialog::OpGeneralDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("General Settings");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("OpGeneralDialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

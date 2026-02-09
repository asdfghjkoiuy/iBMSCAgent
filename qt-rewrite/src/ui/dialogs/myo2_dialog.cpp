#include "ui/dialogs/myo2_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

MyO2Dialog::MyO2Dialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("MyO2");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("MyO2Dialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

#include "ui/dialogs/compare_method_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

CompareMethodDialog::CompareMethodDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Compare Method");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("CompareMethodDialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

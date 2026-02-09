#include "ui/dialogs/import_sm_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

ImportSmDialog::ImportSmDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Import SM");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("ImportSmDialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

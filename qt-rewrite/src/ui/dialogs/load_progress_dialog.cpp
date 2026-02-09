#include "ui/dialogs/load_progress_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

LoadProgressDialog::LoadProgressDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Load Progress");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("LoadProgressDialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

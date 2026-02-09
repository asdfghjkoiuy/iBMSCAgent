#include "ui/dialogs/about_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("About");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("AboutDialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

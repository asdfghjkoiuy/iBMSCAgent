#include "ui/dialogs/splash_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

SplashDialog::SplashDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Splash");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("SplashDialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

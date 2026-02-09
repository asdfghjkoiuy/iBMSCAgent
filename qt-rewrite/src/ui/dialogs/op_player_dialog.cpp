#include "ui/dialogs/op_player_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

OpPlayerDialog::OpPlayerDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Player Options");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("OpPlayerDialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

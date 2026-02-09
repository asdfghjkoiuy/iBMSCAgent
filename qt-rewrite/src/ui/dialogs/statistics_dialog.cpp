#include "ui/dialogs/statistics_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace ibmsc {

StatisticsDialog::StatisticsDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Statistics");
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("StatisticsDialog (parity scaffold)", this));
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

} // namespace ibmsc

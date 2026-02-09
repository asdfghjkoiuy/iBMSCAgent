#pragma once

#include <QDialog>

namespace ibmsc {

class OpVisualDialog : public QDialog {
    Q_OBJECT
public:
    explicit OpVisualDialog(QWidget* parent = nullptr);
};

} // namespace ibmsc

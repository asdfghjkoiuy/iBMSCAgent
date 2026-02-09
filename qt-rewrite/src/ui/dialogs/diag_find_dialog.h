#pragma once

#include <QDialog>
#include <QList>
#include <QSet>
#include <QString>

class QCheckBox;
class QLineEdit;
class QSpinBox;

namespace ibmsc {

class DiagFindDialog : public QDialog {
    Q_OBJECT
public:
    struct Criteria {
        QSet<QString> channels;
        int measureMin = 0;
        int measureMax = 999;
        int labelMin = 1;
        int labelMax = 1295;
        int valueMin = 1;
        int valueMax = 1295;
    };

    explicit DiagFindDialog(QWidget* parent = nullptr);
    Criteria criteria() const;

private:
    void setupChannelButtons();
    static int parseBase36_00_ZZ(const QString& raw, int fallback);

    QSpinBox* m_measureMin = nullptr;
    QSpinBox* m_measureMax = nullptr;
    QLineEdit* m_labelMin = nullptr;
    QLineEdit* m_labelMax = nullptr;
    QSpinBox* m_valueMin = nullptr;
    QSpinBox* m_valueMax = nullptr;
    QList<QCheckBox*> m_channelChecks;
};

} // namespace ibmsc

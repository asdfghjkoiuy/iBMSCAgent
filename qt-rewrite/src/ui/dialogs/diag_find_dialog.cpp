#include "ui/dialogs/diag_find_dialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSpinBox>
#include <QVBoxLayout>

#include <algorithm>

namespace ibmsc {

DiagFindDialog::DiagFindDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Find");
    resize(580, 420);

    auto* root = new QVBoxLayout(this);

    auto* channelBox = new QGroupBox("Channels", this);
    auto* channelLayout = new QVBoxLayout(channelBox);
    auto* channelGrid = new QGridLayout();
    setupChannelButtons();
    for (int i = 0; i < m_channelChecks.size(); ++i) {
        channelGrid->addWidget(m_channelChecks[i], i / 6, i % 6);
    }
    channelLayout->addLayout(channelGrid);
    auto* btnRow = new QHBoxLayout();
    auto* allBtn = new QPushButton("All", channelBox);
    auto* noneBtn = new QPushButton("None", channelBox);
    auto* invBtn = new QPushButton("Invert", channelBox);
    btnRow->addWidget(allBtn);
    btnRow->addWidget(noneBtn);
    btnRow->addWidget(invBtn);
    btnRow->addStretch(1);
    channelLayout->addLayout(btnRow);
    connect(allBtn, &QPushButton::clicked, this, [this]() {
        for (auto* cb : m_channelChecks) cb->setChecked(true);
    });
    connect(noneBtn, &QPushButton::clicked, this, [this]() {
        for (auto* cb : m_channelChecks) cb->setChecked(false);
    });
    connect(invBtn, &QPushButton::clicked, this, [this]() {
        for (auto* cb : m_channelChecks) cb->setChecked(!cb->isChecked());
    });
    root->addWidget(channelBox);

    auto* rangeBox = new QGroupBox("Ranges", this);
    auto* rangeGrid = new QGridLayout(rangeBox);
    m_measureMin = new QSpinBox(rangeBox);
    m_measureMax = new QSpinBox(rangeBox);
    m_measureMin->setRange(0, 9999);
    m_measureMax->setRange(0, 9999);
    m_measureMin->setValue(0);
    m_measureMax->setValue(999);

    m_labelMin = new QLineEdit(rangeBox);
    m_labelMax = new QLineEdit(rangeBox);
    auto* validator = new QRegularExpressionValidator(QRegularExpression("^[0-9A-Za-z]{1,2}$"), rangeBox);
    m_labelMin->setValidator(validator);
    m_labelMax->setValidator(validator);
    m_labelMin->setMaxLength(2);
    m_labelMax->setMaxLength(2);
    m_labelMin->setText("01");
    m_labelMax->setText("ZZ");

    m_valueMin = new QSpinBox(rangeBox);
    m_valueMax = new QSpinBox(rangeBox);
    m_valueMin->setRange(0, 1295);
    m_valueMax->setRange(0, 1295);
    m_valueMin->setValue(1);
    m_valueMax->setValue(1295);

    rangeGrid->addWidget(new QLabel("Measure Min"), 0, 0);
    rangeGrid->addWidget(m_measureMin, 0, 1);
    rangeGrid->addWidget(new QLabel("Measure Max"), 0, 2);
    rangeGrid->addWidget(m_measureMax, 0, 3);

    rangeGrid->addWidget(new QLabel("Label Min (00..ZZ)"), 1, 0);
    rangeGrid->addWidget(m_labelMin, 1, 1);
    rangeGrid->addWidget(new QLabel("Label Max (00..ZZ)"), 1, 2);
    rangeGrid->addWidget(m_labelMax, 1, 3);

    rangeGrid->addWidget(new QLabel("Value Min"), 2, 0);
    rangeGrid->addWidget(m_valueMin, 2, 1);
    rangeGrid->addWidget(new QLabel("Value Max"), 2, 2);
    rangeGrid->addWidget(m_valueMax, 2, 3);

    root->addWidget(rangeBox);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);
}

DiagFindDialog::Criteria DiagFindDialog::criteria() const {
    Criteria c;
    for (auto* cb : m_channelChecks) {
        if (!cb->isChecked()) continue;
        c.channels.insert(cb->property("channel").toString());
    }
    c.measureMin = std::min(m_measureMin->value(), m_measureMax->value());
    c.measureMax = std::max(m_measureMin->value(), m_measureMax->value());
    c.labelMin = std::min(parseBase36_00_ZZ(m_labelMin->text(), 1), parseBase36_00_ZZ(m_labelMax->text(), 1295));
    c.labelMax = std::max(parseBase36_00_ZZ(m_labelMin->text(), 1), parseBase36_00_ZZ(m_labelMax->text(), 1295));
    c.valueMin = std::min(m_valueMin->value(), m_valueMax->value());
    c.valueMax = std::max(m_valueMin->value(), m_valueMax->value());
    return c;
}

void DiagFindDialog::setupChannelButtons() {
    const QList<QPair<QString, QString>> channels = {
        {"BPM", "03"},
        {"STOP", "09"},
        {"SCROLL", "SC"},
        {"A1", "11"}, {"A2", "12"}, {"A3", "13"}, {"A4", "14"},
        {"A5", "15"}, {"A6", "16"}, {"A7", "18"}, {"A8", "19"},
        {"D1", "21"}, {"D2", "22"}, {"D3", "23"}, {"D4", "24"},
        {"D5", "25"}, {"D6", "26"}, {"D7", "28"}, {"D8", "29"},
        {"BGA", "04"}, {"LAYER", "06"}, {"POOR", "07"}, {"BGM", "01"}
    };
    for (const auto& item : channels) {
        auto* cb = new QCheckBox(item.first, this);
        cb->setChecked(true);
        cb->setProperty("channel", item.second);
        m_channelChecks.push_back(cb);
    }
}

int DiagFindDialog::parseBase36_00_ZZ(const QString& raw, int fallback) {
    const QString s = raw.trimmed().toUpper();
    if (s.isEmpty() || s.size() > 2) {
        return fallback;
    }
    bool ok = false;
    const int v = s.toInt(&ok, 36);
    if (!ok) {
        return fallback;
    }
    return std::clamp(v, 0, 1295);
}

} // namespace ibmsc

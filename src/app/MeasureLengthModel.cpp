#include "app/MeasureLengthModel.h"
#include "editor/EditorDocument.h"
#include "model/BmsDocument.h"

namespace App {

MeasureLengthModel::MeasureLengthModel(Editor::EditorDocument* doc, QObject* parent)
    : QAbstractListModel(parent), m_doc(doc)
{
    connect(doc, &Editor::EditorDocument::documentChanged, this, &MeasureLengthModel::refresh);
    refresh();
}

void MeasureLengthModel::refresh() {
    beginResetModel();
    m_measures.clear();
    const auto& lengths = m_doc->document().measureLengths;
    for (const auto& kv : lengths) {
        if (std::fabs(kv.second - 1.0) > 1e-9)
            m_measures.push_back(kv.first);
    }
    std::sort(m_measures.begin(), m_measures.end());
    endResetModel();
}

int MeasureLengthModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_measures.size());
}

QVariant MeasureLengthModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) return {};
    int m = m_measures[index.row()];
    switch (role) {
    case MeasureIndexRole:  return m;
    case LengthRole:        return m_doc->document().measureLength(m);
    case IsNonDefaultRole:  return true;
    default:                return {};
    }
}

QHash<int, QByteArray> MeasureLengthModel::roleNames() const {
    return {
        {MeasureIndexRole, "measureIndex"},
        {LengthRole,       "length"},
        {IsNonDefaultRole, "isNonDefault"},
    };
}

void MeasureLengthModel::setLength(int measureIndex, double length) {
    m_doc->setMeasureLength(measureIndex, length);
}

} // namespace App

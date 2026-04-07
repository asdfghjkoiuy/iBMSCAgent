#pragma once
#include <QAbstractListModel>
#include <QObject>
#include <vector>

namespace Editor { class EditorDocument; }

namespace App {

/// QAbstractListModel exposing per-measure length overrides to QML.
/// Only measures with a non-default (non-1.0) length are shown.
class MeasureLengthModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        MeasureIndexRole = Qt::UserRole + 1,
        LengthRole,
        IsNonDefaultRole,
    };

    explicit MeasureLengthModel(Editor::EditorDocument* doc, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void setLength(int measureIndex, double length);

    void refresh();

private:
    Editor::EditorDocument* m_doc;
    std::vector<int> m_measures; ///< Measure indices with non-default length, sorted
};

} // namespace App

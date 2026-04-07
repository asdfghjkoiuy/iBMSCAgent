#pragma once
#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <vector>

namespace Editor { class EditorDocument; }

namespace App {

enum class ResourceKind { WAV, BMP };

/// QAbstractListModel exposing WAV or BMP resource slots to QML.
/// Roles: slotIndex (int), slotLabel (string), filename (string),
///        isUsed (bool), thumbnailUrl (string, BMP only).
class ResourceTableModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        SlotIndexRole   = Qt::UserRole + 1,
        SlotLabelRole,
        FilenameRole,
        IsUsedRole,
        IsCurrentRole,
        ThumbnailUrlRole,
    };

    explicit ResourceTableModel(ResourceKind kind,
                                Editor::EditorDocument* doc,
                                QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    /// Called from QML delegate onEditingFinished; row is ListView index.
    Q_INVOKABLE void setFilename(int row, const QString& filename);

    /// Rebuild the visible slot list from the current document state.
    void refresh();

private:
    ResourceKind m_kind;
    Editor::EditorDocument* m_doc;
    /// Sorted list of slot indices that are visible in the panel.
    std::vector<int> m_slots;
    /// Base directory for thumbnail resolution (set when doc is loaded).
    QString m_baseDir;
};

} // namespace App

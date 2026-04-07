#include "app/ResourceTableModel.h"
#include "editor/EditorDocument.h"
#include "model/BmsDocument.h"
#include "model/ChannelType.h"
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <set>

namespace App {

using namespace Model;

static const int kMaxSlots = 1296;

ResourceTableModel::ResourceTableModel(ResourceKind kind,
                                       Editor::EditorDocument* doc,
                                       QObject* parent)
    : QAbstractListModel(parent), m_kind(kind), m_doc(doc)
{
    connect(doc, &Editor::EditorDocument::documentChanged, this, &ResourceTableModel::refresh);
    connect(doc, &Editor::EditorDocument::selectedNoteInfoChanged, this, &ResourceTableModel::refresh);
    connect(doc, &Editor::EditorDocument::filePathChanged, this, [this]() {
        m_baseDir = QFileInfo(m_doc->filePath()).absolutePath();
        refresh();
    });
    refresh();
}

void ResourceTableModel::refresh() {
    beginResetModel();
    m_slots.clear();

    const BmsDocument& doc = m_doc->document();

    // Collect slots that are referenced by notes (used in playback/display)
    std::set<int> usedSlots;
    for (const auto& note : doc.notes) {
        ChannelType ct = classifyChannel(note.channelIndex);
        if (ct == ChannelType::BGM || isPlayerNoteChannel(ct)) {
            if (note.value >= 1 && note.value < kMaxSlots)
                usedSlots.insert(note.value);
        }
    }

    // Include slots with a filename OR that are used by notes
    for (int i = 1; i < kMaxSlots; ++i) {
        bool hasFile = (m_kind == ResourceKind::WAV)
                       ? !doc.wavTable[i].empty()
                       : !doc.bmpTable[i].empty();
        if (hasFile || usedSlots.count(i))
            m_slots.push_back(i);
    }

    endResetModel();
}

int ResourceTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_slots.size());
}

QVariant ResourceTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return {};

    int slot = m_slots[index.row()];
    const BmsDocument& doc = m_doc->document();

    switch (role) {
    case SlotIndexRole:
        return slot;
    case SlotLabelRole: {
        // Format as two-digit base-36 (e.g. slot 37 → "11")
        const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        char buf[3] = {digits[slot / 36], digits[slot % 36], '\0'};
        return QString::fromLatin1(buf);
    }
    case FilenameRole:
        return (m_kind == ResourceKind::WAV)
               ? QString::fromStdString(doc.wavTable[slot])
               : QString::fromStdString(doc.bmpTable[slot]);
    case IsUsedRole: {
        for (const auto& note : doc.notes) {
            ChannelType ct = classifyChannel(note.channelIndex);
            if ((ct == ChannelType::BGM || isPlayerNoteChannel(ct)) && note.value == slot)
                return true;
        }
        return false;
    }
    case IsCurrentRole:
        return m_doc->selectedNoteInfo().value("slotIndex").toInt() == slot;
    case ThumbnailUrlRole: {
        if (m_kind != ResourceKind::BMP) return QString();
        const std::string& fname = doc.bmpTable[slot];
        if (fname.empty()) return QString();
        QString path = QDir(m_baseDir).filePath(QString::fromStdString(fname));
        return QFileInfo::exists(path) ? QUrl::fromLocalFile(path).toString() : QString();
    }
    default:
        return {};
    }
}

QHash<int, QByteArray> ResourceTableModel::roleNames() const {
    return {
        {SlotIndexRole,   "slotIndex"},
        {SlotLabelRole,   "slotLabel"},
        {FilenameRole,    "filename"},
        {IsUsedRole,      "isUsed"},
        {IsCurrentRole,   "isCurrent"},
        {ThumbnailUrlRole,"thumbnailUrl"},
    };
}

void ResourceTableModel::setFilename(int row, const QString& filename) {
    if (row < 0 || row >= rowCount()) return;
    int slot = m_slots[row];
    if (m_kind == ResourceKind::WAV)
        m_doc->setWavFilename(slot, filename);
    else
        m_doc->setBmpFilename(slot, filename);
}

} // namespace App

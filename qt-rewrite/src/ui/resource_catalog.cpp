#include "ui/resource_catalog.h"

namespace ibmsc {

QIcon ResourceCatalog::iconByVbName(const QString& vbName) {
    const QString path = QString(":/ibmsc/icons/%1.png").arg(vbName);
    return QIcon(path);
}

bool ResourceCatalog::hasIcon(const QString& vbName) {
    return !iconByVbName(vbName).isNull();
}

} // namespace ibmsc

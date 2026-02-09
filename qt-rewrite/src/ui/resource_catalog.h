#pragma once

#include <QIcon>
#include <QString>

namespace ibmsc {

class ResourceCatalog {
public:
    static QIcon iconByVbName(const QString& vbName);
    static bool hasIcon(const QString& vbName);
};

} // namespace ibmsc

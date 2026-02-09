#pragma once

#include "core/bms_document.h"

namespace ibmsc {

class IUndoCommand {
public:
    virtual ~IUndoCommand() = default;
    virtual void apply(BmsDocument& doc) = 0;
    virtual void revert(BmsDocument& doc) = 0;
};

} // namespace ibmsc

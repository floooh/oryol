#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::GfxEvent
    @brief an event from the Gfx module
    
    Subscribe to GfxEvents with the Gfx::Subscribe() method.
*/
#include "Gfx/Attrs/DisplayAttrs.h"

namespace Oryol {

class GfxEvent {
public:
    /// event types
    enum Type {
        DisplayModified,

        NumTypes,
        InvalidType
    };

    /// default constructor
    GfxEvent();
    /// constructor with arguments
    GfxEvent(Type type, const DisplayAttrs& attrs) : Type(type), DisplayAttrs(attrs) { }

    enum Type Type = InvalidType;
    struct DisplayAttrs DisplayAttrs;
};

} // namespace Oryol

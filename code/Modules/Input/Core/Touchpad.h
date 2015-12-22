#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Touchpad
    @ingroup Input
    @brief access to touchpad state
*/
#include "Core/Types.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/StaticArray.h"
#include "Core/Containers/Map.h"
#include "glm/vec2.hpp"
#include <functional>

namespace Oryol {
    
class Touchpad {
public:
    /// max number of touch positions
    static const int32 MaxNumTouches = 2;

    /// touch event for event-driven input handling (vs polling)
    struct Event {
        /// event type
        enum Type {
            Tapped = 0,
            DoubleTapped,
            PanningStarted,
            Panning,
            PanningEnded,
            PanningCancelled,
            PinchingStarted,
            Pinching,
            PinchingEnded,
            PinchingCancelled,

            NumTypes,
            InvalidType
        } Type;

        /// touch positions
        StaticArray<glm::vec2, MaxNumTouches> Position;
        /// start positions
        StaticArray<glm::vec2, MaxNumTouches> StartPosition;
        /// movement
        StaticArray<glm::vec2, MaxNumTouches> Movement;
    };
    /// unique event handler id typedef
    typedef unsigned int EventHandlerId;
    /// touch event handler typedef
    typedef std::function<void(const Event&)> EventHandler;

    /// constructor
    Touchpad();
    
    /// touchpad valid?
    bool Attached;
    
    /// single-tapped?
    bool Tapped;
    /// double-tapped?
    bool DoubleTapped;
    /// started panning
    bool PanningStarted;
    /// panning gesture underway?
    bool Panning;
    /// ended panning
    bool PanningEnded;
    /// started pinching
    bool PinchingStarted;
    /// pinching underway
    bool Pinching;
    /// ended pinching
    bool PinchingEnded;

    /// current position (1 for tap/pan, 2 for pinch)
    StaticArray<glm::vec2, MaxNumTouches> Position;
    /// current movement (1 for tap/pan, 2 for pinch)
    StaticArray<glm::vec2, MaxNumTouches> Movement;
    /// start position (useful for tap/pan)
    StaticArray<glm::vec2, MaxNumTouches> StartPosition;

    /// called when single-tap detected
    void onTapped(const glm::vec2& pos);
    /// called when double-tap detected
    void onDoubleTapped(const glm::vec2& pos);
    /// called when panning-start detected
    void onPanningStarted(const glm::vec2& pos, const glm::vec2& startPos);
    /// called while panning
    void onPanning(const glm::vec2& pos, const glm::vec2& startPos);
    /// called when panning has ended
    void onPanningEnded(const glm::vec2& pos, const glm::vec2& startPos);
    /// called when panning suddenly ended
    void onPanningCancelled();
    /// called when pinching started
    void onPinchingStarted(const glm::vec2& pos0, const glm::vec2& pos1, const glm::vec2& startPos0, const glm::vec2& startPos1);
    /// called while pinching
    void onPinching(const glm::vec2& pos0, const glm::vec2& pos1, const glm::vec2& startPos0, const glm::vec2& startPos1);
    /// called when pinching ended
    void onPinchingEnded(const glm::vec2& pos0, const glm::vec2& pos1, const glm::vec2& startPos0, const glm::vec2& startPos1);
    /// called when pinching suddenly ended
    void onPinchingCancelled();
    
    /// subscribe to touch events
    EventHandlerId subscribe(EventHandler handler);
    /// unsubscribe from touch events
    void unsubscribe(EventHandlerId id);
    /// reset the touchpad state
    void reset();

private:
    /// notify event handlers
    void notifyEventHandlers(const Event& event);

    EventHandlerId uniqueIdCounter;
    Map<EventHandlerId, EventHandler> eventHandlers;
};

} // namespace Oryol

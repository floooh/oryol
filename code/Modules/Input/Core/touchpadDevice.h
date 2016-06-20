#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::touchpadDevice
    @ingroup _priv
    @brief access to touchpad state
*/
#include "Core/Types.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/StaticArray.h"
#include "Core/Containers/Map.h"
#include "glm/vec2.hpp"
#include <functional>

namespace Oryol {
namespace _priv {

class inputDispatcher;
    
class touchpadDevice {
public:
    /// max number of touch positions
    static const int MaxNumTouches = 2;

    /// constructor
    touchpadDevice();
    
    /// touchpad valid?
    bool attached;
    
    /// single-tapped?
    bool tapped;
    /// double-tapped?
    bool doubleTapped;
    /// started panning
    bool panningStarted;
    /// panning gesture underway?
    bool panning;
    /// ended panning
    bool panningEnded;
    /// started pinching
    bool pinchingStarted;
    /// pinching underway
    bool pinching;
    /// ended pinching
    bool pinchingEnded;

    /// current position (1 for tap/pan, 2 for pinch)
    StaticArray<glm::vec2, MaxNumTouches> position;
    /// current movement (1 for tap/pan, 2 for pinch)
    StaticArray<glm::vec2, MaxNumTouches> movement;
    /// start position (useful for tap/pan)
    StaticArray<glm::vec2, MaxNumTouches> startPosition;

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
    
    /// reset the touchpad state
    void reset();

    inputDispatcher* dispatcher = nullptr;    
};

} // namespace _priv
} // namespace Oryol

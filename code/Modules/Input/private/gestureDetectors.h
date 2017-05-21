#pragma once
//------------------------------------------------------------------------------
#include "Core/Types.h"
#include "Core/Time/TimePoint.h"
#include "glm/vec2.hpp"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::gestureState
    @ingroup _priv
    @brief result of a gesture detector
*/
class gestureState {
public:
    /// gesture action codes
    enum Code {
        none  = 0,
        start = (1<<0),
        move  = (1<<1),
        end   = (1<<2),
        action = start|end,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::touchEvent
    @ingroup _priv
    @brief internal touch event
*/
class touchEvent {
public:
    enum touchType {
        began,
        moved,
        ended,
        cancelled,
        invalid,
    } type = invalid;
    TimePoint time;
    int numTouches = 0;
    static const int MaxNumPoints = 8;
    struct point {
        uintptr_t identifier = 0;     // unique 'finger identifier'
        glm::vec2 pos;
        bool isChanged = false;
    } points[MaxNumPoints];
    const point* findPoint(uintptr_t touchId) const;
    bool sameTouches(const touchEvent& other) const;
    const glm::vec2& touchPos(uintptr_t touchId) const;
};
    
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::panDetector
    @ingroup _priv
    @brief panning gesture detector
*/
class panDetector {
public:
    static const int MinMovedDistance = 10;
    
    gestureState::Code detect(const touchEvent& newEvent);
    void reset();
    glm::vec2 position;
    glm::vec2 startPosition;
    bool distLess(const glm::vec2& pos0, const glm::vec2& pos1, int dist) const;
    
    touchEvent startEvent;
    bool panning = false;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pinchDetector
    @ingroup _priv
    @brief 2-finger-pinch gesture detector
*/
class pinchDetector {
public:
    gestureState::Code detect(const touchEvent& newEvent);
    void reset();

    glm::vec2 position0;
    glm::vec2 position1;
    glm::vec2 startPosition0;
    glm::vec2 startPosition1;
    touchEvent startEvent;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::tapDetector
    @ingroup _priv
    @brief singletap- and multitap-gesture recognizer
*/
class tapDetector {
public:
    static const int TouchTimeout = 250;
    static const int TapTimeout = 750;
    static const int MaxTouchDistance = 20;
    static const int MaxTapDistance = 30;
    
    gestureState::Code detect(const touchEvent& newEvent);
    void reset();
    bool withinDistance(const touchEvent& newEvent, const touchEvent& oldEvent, int maxDist) const;
    bool withinTimeout(const touchEvent& newEvent, const touchEvent& oldEvent, int timeOutMs) const;
    
    int numRequiredTaps = 1;
    glm::vec2 position;
    touchEvent tapEvent;
    int tapCount = 0;
};
} // namespace _priv
} // namespace Oryol

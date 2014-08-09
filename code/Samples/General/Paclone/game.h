#pragma once
//------------------------------------------------------------------------------
/**
    Implement the pacman game loop.
*/
#include "Core/Types.h"
#include "canvas.h"

namespace Paclone {

class game {
public:
    /// constructor
    game();
    
    /// initiliaze (called once)
    void Init(canvas* canvas);
    /// update the game (called per frame)
    void Update(Oryol::int32 tick, canvas* canvas);
    /// cleanup (called once)
    void Cleanup();
    
private:
};
    
} // namespace Paclone
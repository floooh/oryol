#pragma once
//------------------------------------------------------------------------------
/**
    @file core/core.h

    Define core functions for Oryol.
*/
#include "core/types.h"

namespace oryol
{
class core
{
public:
    /// setup the core system
    static void setup();
    /// discard the core system
    static void discard();

private:
    /// early init call from main-thread
    static void init_global_once();
    /// early init call from other threads
    static void init_thread_once();
};
} // namespace oryol
#pragma once
//------------------------------------------------------------------------------
/**
    @file mtl_impl.h
    @brief actually include Metal headers, only include in Obj-C code!
*/
#if !defined(__OBJC__)
#error "Must be included from Objective-C++"
#endif
#include "Gfx/private/mtl/mtl_decl.h"

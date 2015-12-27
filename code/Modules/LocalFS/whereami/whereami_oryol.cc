//------------------------------------------------------------------------------
//  whereami_oryol.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Memory/Memory.h"

#define WAI_MALLOC(size) Oryol::Memory::Alloc(size)
#define WAI_FREE(p) Oryol::Memory::Free(p)
#define WAI_REALLOC(p,size) Oryol::Memory::ReAlloc(p,size)

#include "whereami.h"
#include "whereami.c"


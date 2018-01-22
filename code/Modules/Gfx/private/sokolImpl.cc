//------------------------------------------------------------------------------
//  sokolImpl.cc
//------------------------------------------------------------------------------
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"
#include "Core/Log.h"
#define SOKOL_IMPL
#define SOKOL_ASSERT(c) o_assert_dbg(c)
#define SOKOL_MALLOC(s) Oryol::Memory::Alloc(s)
#define SOKOL_FREE(p) Oryol::Memory::Free(p)
#define SOKOL_LOG(m) Oryol::Log::Info("%s\n",m)
#define SOKOL_UNREACHABLE o_assert_dbg(false)
#if ORYOL_OPENGL
#include "Gfx/private/gl.h"
#endif
#if ORYOL_OPENGLES2
#define SOKOL_GLES2
#elif ORYOL_OPENGLES3
#define SOKOL_GLES3
#elif ORYOL_OPENGL_CORE_PROFILE
#define SOKOL_GLCORE33
#elif ORYOL_D3D11
#define SOKOL_D3D11
#elif ORYOL_METAL
#error "please include sokolImpl.mm for Metal builds"
#endif
#include "sokol_gfx.h"


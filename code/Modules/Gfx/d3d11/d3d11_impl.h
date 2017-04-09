#pragma once
//------------------------------------------------------------------------------
/**
    @file d3d11_impl.h
    @brief includes the D3D11 headers

    Include this file in sources, and d3d11_decl.h in headers.
*/
#define NOMINMAX
#if ORYOL_UWP
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <wrl.h>
#else
#include <d3d11.h>
#endif
#pragma once
//------------------------------------------------------------------------------
/**
    @file d3d11_decl.h
    @brief D3D11 forward declarations

    Try to use this as much as possible in headers instead of including
    the full d3d11 headers.
*/

typedef unsigned long DWORD;
struct HWND__;
typedef HWND__* HWND;

struct IDXGISwapChain;
struct DXGI_SWAP_CHAIN_DESC;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct ID3D11Buffer;

enum D3D11_USAGE;
typedef enum DXGI_FORMAT DXGI_FORMAT;

#pragma once
//------------------------------------------------------------------------------
/**
    @file d3d11_decl.h
    @brief D3D11 forward declarations

    Try to use this as much as possible in headers instead of including
    the full d3d11 headers.
*/
#include "Gfx/private/win/win_decl.h"

struct IDXGISwapChain;
struct IDXGISwapChain1;
struct DXGI_SWAP_CHAIN_DESC;
struct DXGI_SWAP_CHAIN_DESC1;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Resource;
struct ID3D11Texture2D;
struct ID3D11Texture3D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;
struct ID3D11SamplerState;
typedef struct D3D11_TEXTURE2D_DESC D3D11_TEXTURE2D_DESC;

enum D3D11_USAGE;
typedef enum DXGI_FORMAT DXGI_FORMAT;
typedef enum D3D11_CULL_MODE D3D11_CULL_MODE;
typedef enum D3D11_COMPARISON_FUNC D3D11_COMPARISON_FUNC;
typedef enum D3D11_STENCIL_OP D3D11_STENCIL_OP;
typedef enum D3D11_BLEND D3D11_BLEND;
typedef enum D3D11_BLEND_OP D3D11_BLEND_OP;
typedef enum D3D11_INPUT_CLASSIFICATION D3D11_INPUT_CLASSIFICATION;
typedef enum D3D11_FILTER D3D11_FILTER;
typedef enum D3D11_TEXTURE_ADDRESS_MODE D3D11_TEXTURE_ADDRESS_MODE;
typedef enum D3D_PRIMITIVE_TOPOLOGY D3D_PRIMITIVE_TOPOLOGY;
typedef D3D_PRIMITIVE_TOPOLOGY D3D11_PRIMITIVE_TOPOLOGY;
typedef struct D3D11_SUBRESOURCE_DATA D3D11_SUBRESOURCE_DATA;

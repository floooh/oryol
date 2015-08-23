#pragma once
//------------------------------------------------------------------------------
/**
    @file d3d12_decl.h
    @brief D3D12 forward declarations
*/
#include "Gfx/win/win_decl.h"

struct IDXGIFactory4;
struct IDXGISwapChain3;
typedef enum DXGI_FORMAT DXGI_FORMAT;

struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12DescriptorHeap;
struct ID3D12Resource;
struct ID3D12Fence;
struct ID3D12Heap;

typedef enum D3D12_RESOURCE_STATES D3D12_RESOURCE_STATES;
typedef enum D3D12_HEAP_TYPE D3D12_HEAP_TYPE;
typedef enum D3D12_RESOURCE_STATES D3D12_RESOURCE_STATES;
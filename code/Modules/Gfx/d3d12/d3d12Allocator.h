#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Allocator
    @ingroup _priv
    @brief D3D12 resource allocator wrapper

    Just a bunch of ideas:
        - Oryol resources are batch-destroyed by resource label, so we can group
          multiple allocations in one big alloc block, which is
          freed when resource label is destroyed
        - resource memory must stay valid until they are no longer used by 
          the GPU, BUT the actual Oryol resource object slots can be reused
          immediately, only the underlying resource memory must stay valid
        - thus we can 'destroy' resource objects immediately like in the
          traditional renderers, we just make need to make sure that the
          underlying memory isn't released...
        - and that's what the d3d12Allocator takes care of...
        - when a resource label is destroyed, memory belonging to that
          resource label is marked as 'in limbo', and it will only actually
          be released deferred 'd3d12Config::NumFrames' later
*/
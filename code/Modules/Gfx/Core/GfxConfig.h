#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::GfxConfig
    @brief central configuration constants of the Gfx module
*/
#include "Core/Types.h"

namespace Oryol {

class GfxConfig {
public:
    /// default resource pool size
    static const int32 DefaultResourcePoolSize = 128;
    /// default uniform buffer size (only relevant on some platforms)
    static const int32 DefaultGlobalUniformBufferSize = 4 * 1024 * 1024;
    /// default maximum number of draw-calls per frame (only relevant on some platforms)
    static const int32 DefaultMaxDrawCallsPerFrame = (1<<16);
    /// max number of input meshes
    static const int32 MaxNumInputMeshes = 4;
    /// maximum number of primitive groups for one mesh
    static const int32 MaxNumPrimGroups = 8;
    /// max number of shader programs in one program bundle
    static const int32 MaxNumBundlePrograms = 8;
    /// max number of uniform blocks
    static const int32 MaxNumUniformBlocks = 4;
    /// max number of texture faces
    static const int32 MaxNumTextureFaces = 6;
    /// max number of texture mipmaps
    static const int32 MaxNumTextureMipMaps = 12;
    /// maximum number of components in uniform layout
    static const int32 MaxNumUniformLayoutComponents = 16;
    /// maximum number of components in vertex layout
    static const int32 MaxNumVertexLayoutComponents = 16;
    /// maximum number of in-flight frames for Metal
    static const int32 MtlMaxInflightFrames = 3;
};

} // namespace Oryol
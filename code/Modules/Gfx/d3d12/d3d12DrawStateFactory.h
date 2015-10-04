#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12DrawStateFactory
    @ingroup _priv
    @brief D3D12 implementation of drawStateFactory
*/
#include "Gfx/Resource/drawStateFactoryBase.h"
#include "Core/Containers/Map.h"
#include "glm/vec4.hpp"
#include "glm/gtc/epsilon.hpp"

namespace Oryol {
namespace _priv {

class d3d12DrawStateFactory : public drawStateFactoryBase {
public:
    /// setup drawState resource
    ResourceState::Code SetupResource(drawState& ds);
    /// destroy the drawState
    void DestroyResource(drawState& ds);

private:
    /// create pipeline-state-object and set into drawState
    void createPSO(drawState& ds);
    /// release a pipeline-state-object
    void releasePSO(ID3D12PipelineState* pso);

    // key structure for PSO cache, two cache entries are identical
    // when  all DrawStateSetup components are identical:
    // BlendState, DepthStencilState, RasterizerState hashes, 
    // shader and shader selection mask, hash of the vertex layout
    // ...hmm, that's a big-ass key... 
    struct PsoCacheKey {
        uint64 blendStateHash = 0;
        uint64 depthStencilStateHash = 0;
        uint64 vertexLayoutHash = 0;
        Id shader;
        uint32 shaderSelectionMask = 0;
        uint32 rasterizerStateHash = 0;
        glm::vec4 blendColor;

        bool operator==(const PsoCacheKey& rhs) const {
            return (this->blendStateHash == rhs.blendStateHash) &&
                (this->depthStencilStateHash == rhs.depthStencilStateHash) &&
                (this->vertexLayoutHash == rhs.vertexLayoutHash) &&
                (this->shader == rhs.shader) &&
                (this->shaderSelectionMask == rhs.shaderSelectionMask) &&
                (this->rasterizerStateHash == rhs.rasterizerStateHash) &&
                (glm::all(glm::epsilonEqual(this->blendColor, rhs.blendColor, 0.001f)));
        }

        bool operator<(const PsoCacheKey& rhs) const {
            return (this->blendStateHash < rhs.blendStateHash) ||
                (this->depthStencilStateHash < rhs.depthStencilStateHash) ||
                (this->vertexLayoutHash < rhs.vertexLayoutHash) ||
                (this->shader < rhs.shader) ||
                (this->shaderSelectionMask < rhs.shaderSelectionMask) ||
                (this->rasterizerStateHash < rhs.rasterizerStateHash) ||
                (this->blendColor.x < rhs.blendColor.x) ||
                (this->blendColor.y < rhs.blendColor.y) ||
                (this->blendColor.z < rhs.blendColor.z) ||
                (this->blendColor.w < rhs.blendColor.w);
        }
    };
    /// initialize a PSO cache key from a DrawStateSetup
    PsoCacheKey initPsoCacheKey(const drawState& ds);

    // a PSO cache entry, since d3d12 PSOs are deferred-released (after they
    // are no longer used by GPU), we cannot use the D3D12 object's refcount
    // and need to track a separate usecount to decide when a cache entry
    // can be removed
    struct PsoCacheEntry {
        PsoCacheEntry(ID3D12PipelineState* pso) : d3d12PipelineState(pso), useCount(1) {};
        ID3D12PipelineState* d3d12PipelineState = nullptr;
        int32 useCount = 0;
    };

    // the pipeline-state-object cache, so we don't waste time creating redundant PSOs
    Map<PsoCacheKey, PsoCacheEntry> psoCache;
};

} // namespace _priv
} // namespace Oryol
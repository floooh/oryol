#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlDrawStateFactory
    @ingroup _priv
    @brief Metal implementation of drawStateFactory
*/
#include "Gfx/Resource/drawStateFactoryBase.h"
#include "Resource/Id.h"
#include "Core/Containers/Map.h"
#include "glm/vec4.hpp"
#include "glm/gtc/epsilon.hpp"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class mtlDrawStateFactory : public drawStateFactoryBase {
public:
    /// setup a new drawState resource
    ResourceState::Code SetupResource(drawState& ds);
    /// destroy drawState resource
    void DestroyResource(drawState& ds);

private:
    /// create a MTLRenderPipelineState object in drawState
    void createRPS(drawState& ds);
    /// release MTLRenderPipelineState object in drawState
    void releaseRPS(drawState& ds);
    /// create MTLDepthStencilState object in drawState
    void createDSS(drawState& ds);
    /// release MTLDepthStencilState object in drawState
    void releaseDSS(drawState& ds);

    /// key type for renderPipelineStateCache
    struct RPSKey {
        uint64 blendStateHash = 0;
        uint64 vertexLayoutHash = 0;
        Id shader;
        uint32 shaderSelectionMask = 0;
        uint32 rasterizerStateHash = 0;
        glm::vec4 blendColor;

        bool operator==(const RPSKey& rhs) const {
            return (this->blendStateHash == rhs.blendStateHash) &&
                (this->vertexLayoutHash == rhs.vertexLayoutHash) &&
                (this->shader == rhs.shader) &&
                (this->shaderSelectionMask == rhs.shaderSelectionMask) &&
                (this->rasterizerStateHash == rhs.rasterizerStateHash) &&
                (glm::all(glm::epsilonEqual(this->blendColor, rhs.blendColor, 0.001f)));
        };
        bool operator<(const RPSKey& rhs) const {
            return (this->blendStateHash < rhs.blendStateHash) ||
                (this->vertexLayoutHash < rhs.vertexLayoutHash) ||
                (this->shader < rhs.shader) ||
                (this->shaderSelectionMask < rhs.shaderSelectionMask) ||
                (this->rasterizerStateHash < rhs.rasterizerStateHash) ||
                (this->blendColor.x < rhs.blendColor.x) ||
                (this->blendColor.y < rhs.blendColor.y) ||
                (this->blendColor.z < rhs.blendColor.z) ||
                (this->blendColor.w < rhs.blendColor.w);
        };
    };
    /// initialize a new RPSKey object
    RPSKey initRPSKey(const drawState& ds);

    /// value type for render-pipeline-state-cache
    struct RPSValue {
        ORYOL_OBJC_TYPED_ID(MTLRenderPipelineState) rps;
        int32 useCount;
    };
    /// value type for depth-stencil-state-cache
    struct DSSValue {
        ORYOL_OBJC_TYPED_ID(MTLDepthStencilState) dss;
        int32 useCount;
    };

    /// cache for MTLRenderPipelineState objects
    Map<RPSKey, RPSValue> rpsCache;
    /// cache for DepthStencilState objects
    Map<uint64, DSSValue> dssCache;
};

} // namespace _priv
} // namespace Oryol

#pragma once
//------------------------------------------------------------------------------
#include "Core/Types.h"
#include "Core/Assertion.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"
#include "Resource/Id.h"
#include "Resource/Locator.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/GfxConfig.h"
#include "glm/vec4.hpp"
#include <initializer_list>

namespace Oryol {

//------------------------------------------------------------------------------
/**
    @class Oryol::IndexType
    @ingroup Gfx
    @brief selects 16- or 32-bit indices
*/
class IndexType {
public:
    enum Code {
        None = 0,
        Index16,
        Index32,

        NumIndexTypes,
        InvalidIndexType = 0xFFFFFFFF
    };
    /// get byte size of index type
    static int ByteSize(IndexType::Code c);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PixelChannel
    @ingroup Gfx
    @brief RGBA/Depth/Stencil channel bits and combinations
 */
class PixelChannel {
public:
    typedef uint64_t Mask;
    enum Bits {
        None    = 0,

        Stencil = (1<<5),
        Depth   = (1<<4),
        Red     = (1<<3),
        Green   = (1<<2),
        Blue    = (1<<1),
        Alpha   = (1<<0),

        DepthStencil = Depth|Stencil,
        DS = DepthStencil,

        D = Depth,
        S = Stencil,
        R = Red,
        G = Green,
        B = Blue,
        A = Alpha,

        RG  = R|G,
        GB  = G|B,
        RB  = R|B,
        RGB = R|G|B,

        RA = R|A,
        GA = G|A,
        BA = B|A,

        RGA = R|G|A,
        GBA = G|B|A,
        RBA = R|B|A,

        RGBA = R|G|B|A,
        All = R|G|B|A|D|S,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PixelFormat
    @ingroup Gfx
    @brief enum of pixel formats
*/
class PixelFormat {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint64_t {
    #else
    enum Code {
    #endif
        RGBA8,          ///< 32-bit wide, 4 channels @ 8-bit
        RGB8,           ///< 24-bit wide, 3 channels @ 8-bit
        RGBA4,          ///< 16-bit wide, 4 channels @ 4-bit
        R5G6B5,         ///< 16-bit wide, 3 channels @ 5/6/5 bits
        R5G5B5A1,       ///< 16-bit wide, 4 channels @ 1-bit alpha, 5-bit rgb
        R10G10B10A2,    ///< 32-bit wide, 4 channels @ 10/10/10/2 bits
        RGBA32F,        ///< 128-bit wide, 4 channel @ 32-bit float
        RGBA16F,        ///< 64-bit wide, 4 channel @ 16-bit float
        R32F,           ///< 32-bit wide, 1 channel @ 32-bit float
        R16F,           ///< 16-bit wide, 1 channel @ 16-bit float

        L8,             ///< 8-bit wide, single channel
        DXT1,           ///< DXT1 compressed format
        DXT3,           ///< DXT3 compressed format
        DXT5,           ///< DXT5 compressed format
        DEPTH,          ///< depth-only format (at least 16 bit)
        DEPTHSTENCIL,   ///< depth-stencil format (usually D24S8)
        PVRTC2_RGB,     ///< PVRTC2 compressed format (RGB)
        PVRTC4_RGB,     ///< PVRTC4 compressed format (RGB)
        PVRTC2_RGBA,    ///< PVRTC2 compressed format (RGBA)
        PVRTC4_RGBA,    ///< PVRTC4 compressed format (RGBA)
        ETC2_RGB8,      ///< ETC2 compressed format (RGB8)
        ETC2_SRGB8,     ///< ETC2 compressed format (SRGB8)

        NumPixelFormats,            ///< number of pixel formats
        InvalidPixelFormat,         ///< invalid pixel format value
        None = InvalidPixelFormat,  ///< special "none" type
    };

    /// return true for valid render target color formats
    /// FIXME: hmm according to GLES3 spec, the float formats
    /// are not renderable... does this need a separate extension?
    static bool IsValidRenderTargetColorFormat(Code c);
    /// return true for valid render target depth formats
    static bool IsValidRenderTargetDepthFormat(Code c);
    /// return true for valid color texture formats
    static bool IsValidTextureColorFormat(Code c);
    /// return true for valid depth texture formats
    static bool IsValidTextureDepthFormat(Code c);
    /// test if the pixel format is a pure depth format (not a depth/stencil format)
    static bool IsDepthFormat(Code c);
    /// test if the pixel format is a depth/stencil format
    static bool IsDepthStencilFormat(Code c);
    /// return true if the pixel format is a compressed format
    static bool IsCompressedFormat(Code c);
    /// return true if this is a PVRTC-compressed format
    static bool IsPVRTC(Code c);
    /// return true if this is a DXT-compressed format
    static bool IsDXT(Code c);
    /// return true if this is an ETC-compressed format
    static bool IsETC2(Code c);
    /// get byte size of pixel format
    static int ByteSize(Code c);
    /// get number of bits in a pixel format channel (only for non-compressed formats and non-depth formats!)
    static int8_t NumBits(Code pixelFormat, PixelChannel::Bits channel);
    /// compute row-pitch (distance in bytes from one row of data to next)
    static int RowPitch(PixelFormat::Code fmt, int width);
    /// compute image-pitch (distance in bytes from one image to next)
    static int ImagePitch(PixelFormat::Code fmt, int width, int height);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PrimitiveType
    @ingroup Gfx
    @brief primitive type enum (triangle strips, lists, etc...)
*/
class PrimitiveType {
public:
    /// primitive type enum (don't change order, append to end!)
    enum Code {
        Points = 0,
        Lines,
        LineStrip,
        Triangles,
        TriangleStrip,

        NumPrimitiveTypes,
        InvalidPrimitiveType = 0xFFFFFFFF,
    };
    /// convert primitive type to string
    static const char* ToString(PrimitiveType::Code c);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxResourceType
    @ingroup Gfx
    @brief Gfx module resource types

    These types are used for the type in Id for Gfx module
    resources.
*/
class GfxResourceType {
public:
    /// type enum
    enum Code {
        Texture,            ///< a texture
        Mesh,               ///< a mesh
        Shader,             ///< a shader
        Pipeline,           ///< a pipeline state object
        RenderPass,         ///< a render-pass object

        NumResourceTypes,
        InvalidResourceType = 0xFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderStage
    @ingroup Gfx
    @brief the shader stages (vertex shader, fragment shader)
*/
class ShaderStage {
public:
    /// shader stages enum
    enum Code {
        VS = 0,
        FS,

        NumShaderStages,
        InvalidShaderStage = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureFilterMode
    @ingroup Gfx
    @brief texture sampling filter mode
*/
class TextureFilterMode {
public:
    /// filtering modes
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint16_t {
    #else
    enum Code {
    #endif
        Nearest,
        Linear,
        NearestMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapNearest,
        LinearMipmapLinear,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureType
    @ingroup Gfx
    @brief texture type (2D, 3D, Cube)
*/
class TextureType {
public:
    /// texture type enum
    enum Code {
        Texture2D = 0,
        TextureCube,
        Texture3D,
        TextureArray,

        NumTextureTypes,
        InvalidTextureType = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureWrapMode
    @ingroup Gfx
    @brief texture coordinate wrapping modes
*/
class TextureWrapMode {
public:
    /// wrap modes
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint16_t {
    #else
    enum Code {
    #endif
        ClampToEdge,
        Repeat,
        MirroredRepeat,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Usage
    @ingroup Gfx
    @brief graphics resource usage types

    - Immutable:    requires initialization data
    - Dynamic:      update infrequently
    - Stream:       changed every frame
*/
class Usage {
public:
    /// usage enum
    enum Code {
        Immutable = 0,
        Dynamic,
        Stream,

        NumUsages,
        InvalidUsage = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexAttr
    @ingroup Gfx
    @brief vertex attribute enum (position, texcoord, ...)

    The VertexAttr definitions don't have a hardwired meaning, they just
    exist to make the binding of vertex components (living in vertex buffers)
    to vertex attribute definition in vertex shaders easier to understand.
    The maximum number of vertex attributes should not exceed 16
    (this is the GL_MAX_VERTEX_ATTRIBS value).
*/
class VertexAttr {
public:
    /// vertex attribute enum
    enum Code : uint8_t {
        Position = 0,   ///< "position"
        Normal,         ///< "normal"
        TexCoord0,      ///< "texcoord0"
        TexCoord1,      ///< "texcoord1"
        TexCoord2,      ///< "texcoord2"
        TexCoord3,      ///< "texcoord3"
        Tangent,        ///< "tangent
        Binormal,       ///< "binormal"
        Weights,        ///< "weights" (skin weights)
        Indices,        ///< "indices" (skin indices)
        Color0,         ///< "color0"
        Color1,         ///< "color1"
        Instance0,      ///< "instance0"
        Instance1,      ///< "instance1"
        Instance2,      ///< "instance2"
        Instance3,      ///< "instance3"

        NumVertexAttrs,
        InvalidVertexAttr,
    };

    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexFormat
    @ingroup Gfx
    @brief vertex component formats
    @see Vertex
    
    NOTE: The un-normalized integer formats are not compatible across
    GLES2 and D3D11! GLES2 needs to read those as float vec, but D3D11 
    can only read them as int vec!
*/
class VertexFormat {
public:
    /// format enum (don't change order, and append to end!)
    enum Code : uint8_t {
        Float,          ///< single component float, expanded to (x, 0, 0, 1)
        Float2,         ///< 2-component float, expanded to (x, y, 0, 1)
        Float3,         ///< 3-component float, expanded to (x, y, z, 1)
        Float4,         ///< 4-component float
        Byte4,          ///< 4-component float (-128.0f..+127.0f) mapped to byte (-128..+127)
        Byte4N,         ///< 4-component float (-1.0f..+1.0f) mapped to byte (-128..+127)
        UByte4,         ///< 4-component float (0.0f..255.0f) mapped to byte (0..255)
        UByte4N,        ///< 4-component float (0.0f..+1.0) mapped to byte (-128..+127)
        Short2,         ///< 2-component float (-32768.0f..+32767.0f) mapped to short (-32768..+32767)
        Short2N,        ///< 2-component float (-1.0f..+1.0f) mapped to short (-32768..+32767)
        Short4,         ///< 4-component float (-32768.0f..+32767.0f) mapped to short (-32768..+32767)
        Short4N,        ///< 4-component float (-1.0f..+1.0f) mapped to short (-32768..+32767)
        UInt10_2N,      ///< 4-component packed, normalized 10-bit XYZ, 2-bit W (0.0 .. 1.0)

        NumVertexFormats,       ///< number of vertex formats
        InvalidVertexFormat,    ///< the invalid vertex format value
    };

    /// get the byte size of a vertex format code
    static int ByteSize(Code c);
    // convert to string
    static const char* ToString(Code c);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderLang
    @ingroup Gfx
    @brief shader language syntax
*/
class ShaderLang {
public:
    enum Code {
        GLSL100 = 0,    ///< OpenGLES 2.0 / WebGL 1.0
        GLSL330,        ///< OpenGL 3.3
        GLSLES3,        ///< OpenGLES3
        HLSL5,          ///< D3D11 HLSL
        Metal,          ///< Metal shader language

        NumShaderLangs,
        InvalidShaderLang
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxFeature
    @ingroup Gfx
    @brief optional rendering features
*/
class GfxFeature {
public:
    enum Code {
        TextureCompressionDXT = 0,  ///< GPU supports DXT compressed textures
        TextureCompressionPVRTC,    ///< GPU supports PVRTC compressed textures
        TextureCompressionATC,      ///< GPU supports ATC compressed textures
        TextureCompressionETC2,     ///< GPU supports ETC2 compressed textures (OpenGLES3)
        TextureFloat,               ///< support for float textures
        TextureHalfFloat,           ///< support for half-float textures
        Instancing,                 ///< supports hardware-instanced rendering
        OriginBottomLeft,           ///< image space origin is bottom-left (GL-style)
        OriginTopLeft,              ///< image space origin is top-left (D3D-style)
        MSAARenderTargets,          ///< MSAA support in offscreen-render-targets
        PackedVertexFormat_10_2,    ///< support for 10.10.10.2 bit packed vertex formats
        MultipleRenderTarget,       ///< support for MRT offscreen rendering
        Texture3D,                  ///< support for 3D textures
        TextureArray,               ///< support for array textures
        NativeTexture,              ///< can work with externally created texture objects

        NumFeatures,
        InvalidFeature
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Face
    @ingroup Gfx
    @brief polygon face side (front, back, both)
*/
class Face {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint16_t {
    #else
    enum Code {
    #endif
        Front = 0,
        Back,
        Both,
    };
    static const int NumFaceCodes = 3;
    static const int NumSides = 2;
    static const int InvalidFace = 0xFF;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::CompareFunc
    @ingroup Gfx
    @brief comparison modes for depth and stencil state
*/
class CompareFunc {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint16_t {
    #else
    enum Code {
    #endif
        Never = 0,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };
    static const int NumCompareFuncs = 8;
    static const int InvalidCompareFunc = 0xFF;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::StencilOp
    @ingroup Gfx
    @brief stencil operations
*/
class StencilOp {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint16_t {
    #else
    enum Code {
    #endif
        Keep,
        Zero,
        Replace,
        IncrClamp,
        DecrClamp,
        Invert,
        IncrWrap,
        DecrWrap,
    };
    static const int NumStencilOperations = 8;
    static const int InvalidStencilOperation = 0xff;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BlendFactor
    @ingroup Gfx
    @brief blending factors
*/
class BlendFactor {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint64_t {
    #else
    enum Code {
    #endif
        Zero = 0,
        One,
        SrcColor,
        OneMinusSrcColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstColor,
        OneMinusDstColor,
        DstAlpha,
        OneMinusDstAlpha,
        SrcAlphaSaturated,
        BlendColor,
        OneMinusBlendColor,
        BlendAlpha,
        OneMinusBlendAlpha,
    };
    static const int NumBlendFactors = 15;
    static const int InvalidBlendFactor = 0xFF;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BlendOperation
    @ingroup Gfx
    @brief blending operations
*/
class BlendOperation {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint64_t {
    #else
    enum Code {
    #endif
        Add = 0,
        Subtract,
        ReverseSubtract,
    };
    static const int NumBlendOperations = 3;
    static const int InvalidBlendOperation = 0xff;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexStepFunction
    @ingroup Gfx
    @brief classify vertices in a buffer as per-vertex or per-instance data
*/
class VertexStepFunction {
public:
    enum Code : uint8_t {
        PerVertex = 0,
        PerInstance = 1,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PrimitiveGroup
    @ingroup Gfx
    @brief describes a group of primitives to be rendered
    
    A PrimitiveGroup object describes a range of primitive elements in
    a mesh, where elements are either vertices or indices.
*/
class PrimitiveGroup {
public:
    int BaseElement = 0;
    int NumElements = 0;

    /// default constructor
    PrimitiveGroup() {};
    /// construct for indexed or non-indexed
    PrimitiveGroup(int baseElement, int numElements) :
        BaseElement(baseElement),
        NumElements(numElements) { }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BlendState
    @ingroup Gfx
    @brief describe alpha blending state
*/
class BlendState {
public:
    union {
        #pragma pack(push,1)
        struct {
            uint64_t BlendEnabled:1;
            BlendFactor::Code SrcFactorRGB:5;
            BlendFactor::Code DstFactorRGB:5;
            BlendOperation::Code OpRGB:3;
            BlendFactor::Code SrcFactorAlpha:5;
            BlendFactor::Code DstFactorAlpha:5;
            BlendOperation::Code OpAlpha:3;
            PixelChannel::Mask ColorWriteMask:4;
            PixelFormat::Code ColorFormat : 5;
            PixelFormat::Code DepthFormat : 5;
            uint64_t MRTCount : 3;
        };
        #pragma pack(pop)
        /// hash code from merged state
        uint64_t Hash;
    };

    /// constructor
    BlendState();
    /// equality
    bool operator==(const BlendState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const BlendState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::StencilState
    @ingroup Gfx
    @brief holds stencil-buffer render state for one face side
*/
class StencilState {
public:
    union {
        #pragma pack(push, 1)
        struct {
            StencilOp::Code FailOp : 4;
            StencilOp::Code DepthFailOp : 4;
            StencilOp::Code PassOp : 4;
            CompareFunc::Code CmpFunc : 4;
        };
        #pragma pack(pop)
        uint16_t Hash;
    };
    /// constructor
    StencilState();
    /// equality
    bool operator==(const StencilState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const StencilState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::DepthStencilState
    @ingroup Gfx
    @brief holds the complete depth and stencil render state
*/
class DepthStencilState {
public:
    /// front-side stencil state
    StencilState StencilFront;
    /// back-side stencil state
    StencilState StencilBack;
    /// common depth-stencil state
    union {
        struct {
            /// depth compare-function
            CompareFunc::Code DepthCmpFunc:5;
            /// depth write enabled flag
            uint16_t DepthWriteEnabled:1;
            /// stencil-enabled flag
            uint16_t StencilEnabled:1;
            /// stencil read-mask
            uint16_t StencilReadMask : 8;
            /// stencil write-mask
            uint16_t StencilWriteMask : 8;
            /// stencil-ref value
            uint16_t StencilRef : 8;
        };
        uint32_t Hash;
    };
    /// constructor
    DepthStencilState();
    /// equality
    bool operator==(const DepthStencilState& rhs) const;
    /// inequality
    bool operator!=(const DepthStencilState& rhs) const;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::RasterizerState
    @ingroup Gfx
    @brief rasterizer state flags
*/
class RasterizerState {
public:
    union {
        #pragma pack(push,1)
        struct {
            uint16_t CullFaceEnabled : 1;
            uint16_t ScissorTestEnabled : 1;
            uint16_t DitherEnabled : 1;
            uint16_t AlphaToCoverageEnabled : 1;
            Face::Code CullFace : 3;
            uint16_t SampleCount : 4;
        };
        #pragma pack(pop)
        uint16_t Hash;
    };
    /// constructor
    RasterizerState();
    /// equality
    bool operator==(const RasterizerState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const RasterizerState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::SamplerState
    @ingroup Gfx
    @brief wrap texture sampler state
*/
class SamplerState {
public:
    union {
        #pragma pack(push, 1)
        struct {
            /// texture-wrap mode for u-axis
            TextureWrapMode::Code WrapU : 2;
            /// texture-wrap mode for v-axis
            TextureWrapMode::Code WrapV : 2;
            /// texture-wrap mode for w-axis
            TextureWrapMode::Code WrapW : 2;
            /// magnification filter
            TextureFilterMode::Code MagFilter : 3;
            /// minification filter
            TextureFilterMode::Code MinFilter : 3;
        };
        #pragma pack(pop)
        uint16_t Hash;
    };
    /// constructor
    SamplerState();
    /// equality
    bool operator==(const SamplerState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const SamplerState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PassAction
    @ingroup Gfx
    @brief what happens at BeginPass() 
*/
class PassAction {
public:
    /// default constructor, set all actions to 'clear with default values'
    PassAction();
    /// clear all surfaces with given values
    static PassAction Clear(const glm::vec4& color=glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), float depth=1.0f, uint8_t stencil=0);
    /// clear all surfaces with individual colors
    static PassAction Clear(std::initializer_list<glm::vec4> colors, float depth=1.0f, uint8_t stencil=0);
    /// load previous content
    static PassAction Load();
    /// discard previous content
    static PassAction DontCare();

    /// FIXME: these methods are confusing, since some are static, some are not!
    /// clear a single surface to a color
    PassAction& ClearColor(int index, const glm::vec4& color);
    /// clear depth-stencil surface  
    PassAction& ClearDepthStencil(float depth=1.0f, uint8_t stencil=0); 
    /// set a color surface to 'dont care' (initial content is undefined)
    PassAction& DontCareColor(int index);
    /// set depth-stencil initial state to 'dont care'
    PassAction& DontCareDepthStencil();
    /// initialize color surface with its previus content
    PassAction& LoadColor(int index);
    /// initialize depth-stencil surface with its previous content
    PassAction& LoadDepthStencil();

    /// override clear colors
    StaticArray<glm::vec4, GfxConfig::MaxNumColorAttachments> Color;
    /// override clear depth value
    float Depth = 1.0f;
    /// override clear-stencil value
    uint8_t Stencil = 0;

    /// action bits (not set: don't care)
    enum Action {
        ClearC0 = (1<<0),
        ClearC1 = (1<<1),
        ClearC2 = (1<<2),
        ClearC3 = (1<<3),

        LoadC0  = (1<<4),
        LoadC1  = (1<<5),
        LoadC2  = (1<<6),
        LoadC3  = (1<<7),

        ClearDS = (1<<8),
        LoadDS  = (1<<9),
    };
    uint16_t Flags = ClearC0|ClearC1|ClearC2|ClearC3|ClearDS;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::DrawState
    @brief state required to issue draw calls
    
    The DrawState struct contains state required to issue draw calls
    with the exception of shader uniforms:
    
    - 1 pipeline state object
    - 1..4 mesh objects
    - 0..N textures for the vertex shader stage
    - 0..N textures for the fragment shader stage
*/
struct DrawState {
    /// the pipeline state object
    Id Pipeline;
    /// input meshes
    StaticArray<Id, GfxConfig::MaxNumInputMeshes> Mesh;
    /// vertex shader stage textures
    StaticArray<Id, GfxConfig::MaxNumVertexTextures> VSTexture;
    /// fragment shader stage textures
    StaticArray<Id, GfxConfig::MaxNumFragmentTextures> FSTexture;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxFrameInfo
    @brief per-frame stats of the Gfx module
*/
struct GfxFrameInfo {
    int NumPasses = 0;
    int NumApplyViewPort = 0;
    int NumApplyScissorRect = 0;
    int NumApplyDrawState = 0;
    int NumApplyUniformBlock = 0;
    int NumUpdateVertices = 0;
    int NumUpdateIndices = 0;
    int NumUpdateTextures = 0;
    int NumDraw = 0;
    int NumDrawInstanced = 0;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexLayout
    @ingroup Gfx
    @brief describes the data layout of a vertex in a vertex buffer
*/
class VertexLayout {
public:
    /// a component in a vertex layout
    #pragma pack(push,1)
    class Component {
    public:
        /// default constructor
        Component() {};
        /// construct from vertex attr and format
        Component(VertexAttr::Code attr, VertexFormat::Code fmt) : Attr(attr), Format(fmt) { }
        /// return true if valid (attr and format set)
        bool IsValid() const;
        /// clear the component (unset attr and format)
        void Clear();
        /// get byte size of component
        int ByteSize() const;

        VertexAttr::Code Attr = VertexAttr::InvalidVertexAttr;
        VertexFormat::Code Format = VertexFormat::InvalidVertexFormat;
    };
    #pragma pack(pop)

    /// the vertex step function, used for instancing, default is 'PerVertex'
    VertexStepFunction::Code StepFunction = VertexStepFunction::PerVertex;
    /// the vertex step rate, used for instancing
    uint8_t StepRate = 1;

    /// default constructor
    VertexLayout();
    /// initializer list constructor
    VertexLayout(std::initializer_list<Component> l);
    /// clear the vertex layout, chainable
    VertexLayout& Clear();
    /// return true if layout is empty
    bool Empty() const;
    /// add a component
    VertexLayout& Add(const Component& comp);
    /// add component by name and format
    VertexLayout& Add(VertexAttr::Code attr, VertexFormat::Code format);
    /// add multiple components via initializer list
    VertexLayout& Add(std::initializer_list<Component> l);
    /// enable layout for instancing, set StepFunction to PerInstance and StepRate to 1
    VertexLayout& EnableInstancing();
    /// get number of components
    int NumComponents() const;
    /// get component at index
    const Component& ComponentAt(int index) const;
    /// get component index by vertex attribute, return InvalidIndex if layout doesn't include attr
    int ComponentIndexByVertexAttr(VertexAttr::Code attr) const;
    /// get byte size of vertex (aka stride)
    int ByteSize() const;
    /// get byte offset of a component
    int ComponentByteOffset(int componentIndex) const;
    /// test if the layout contains a specific vertex attribute
    bool Contains(VertexAttr::Code attr) const;
private:
    StaticArray<Component, GfxConfig::MaxNumVertexLayoutComponents> comps;
    StaticArray<uint8_t, GfxConfig::MaxNumVertexLayoutComponents> byteOffsets;
    StaticArray<int8_t, VertexAttr::NumVertexAttrs> attrCompIndices;  // maps vertex attributes to component indices
    int8_t numComps = 0;
    uint8_t byteSize = 0;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureAttrs
    @ingroup Gfx
    @brief holds the public attributes of a Texture object
    
    @todo: describe TextureAttrs
*/
struct TextureAttrs {
    /// texture locator (usually the URL of the texture file)
    class Locator Locator;
    /// the texture type (2D, 3D, cube...)
    TextureType::Code Type = TextureType::InvalidTextureType;
    /// the RGBA pixel format of texture data
    PixelFormat::Code ColorFormat = PixelFormat::InvalidPixelFormat;
    /// optional depth format (only used for render target textures)
    PixelFormat::Code DepthFormat = PixelFormat::InvalidPixelFormat;
    /// optional sample count (only used for MSAA render target textures)
    int SampleCount = 1;
    /// texture usage hint
    Usage::Code TextureUsage = Usage::InvalidUsage;
    /// width of top-level mipmap in pixels
    int Width = 0;
    /// height of top-level mipmap in pixels
    int Height = 0;
    /// depth of top-level mipmap in pixels (only used for 3D textures)
    int Depth = 0;
    /// number of mipmaps (1 for 'no child mipmaps')
    int NumMipMaps = 1;
    /// true if this is a render target texture
    bool IsRenderTarget = false;
    /// true if this render target texture has an attached depth buffer
    bool HasDepthBuffer = false;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::DisplayAttrs
    @ingroup Gfx
    @brief actual display attributes
    
    Holds the properties of the current display, these can be 
    different from the display setup parameters.
*/
struct DisplayAttrs {
    /// window width (including window chrome)
    int WindowWidth = 0;
    /// window height (including window chrome)
    int WindowHeight = 0;
    /// x-position of window
    int WindowPosX = 0;
    /// y-position of window
    int WindowPosY = 0;
    /// width of framebuffer associated with window
    int FramebufferWidth = 0;
    /// height of framebuffer associated with window
    int FramebufferHeight = 0;
    /// framebuffer pixel format
    PixelFormat::Code ColorPixelFormat = PixelFormat::RGBA8;
    /// depth buffer pixel format (PixelFormat::None if no depth buffer)
    PixelFormat::Code DepthPixelFormat = PixelFormat::DEPTHSTENCIL;
    /// number of multisample-anti-aliasing samples
    int SampleCount = 1;
    /// indicates windowed or fullscreen mode
    bool Windowed = true;
    /// vsync swap interval (0 means: no vsync)
    int SwapInterval = 1;
    /// window title as UTF-8
    String WindowTitle;

    /// init a DisplayAttrs object from a TextureAttrs object
    static DisplayAttrs FromTextureAttrs(const TextureAttrs& texAttrs);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxEvent
    @brief an event from the Gfx module
    
    Subscribe to GfxEvents with the Gfx::Subscribe() method.
*/
class GfxEvent {
public:
    /// event types
    enum Type {
        DisplayModified,

        NumTypes,
        InvalidType
    };
    /// default constructor
    GfxEvent();
    /// constructor with arguments
    GfxEvent(Type type, const DisplayAttrs& attrs) : Type(type), DisplayAttrs(attrs) { }

    enum Type Type = InvalidType;
    struct DisplayAttrs DisplayAttrs;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ImageDataAttrs
    @brief describe offsets and sizes of image surfaces
*/
class ImageDataAttrs {
public:
    /// constructor
    ImageDataAttrs();
    /// number of faces
    int NumFaces = 0;
    /// number of mipmaps
    int NumMipMaps = 0;
    /// pixel data mipmap image offsets
    StaticArray<StaticArray<int, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Offsets;
    /// pixel data mipmap image sizes
    StaticArray<StaticArray<int, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Sizes;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::IndexBufferAttrs
    @ingroup Gfx
    @brief attributes of an index buffer
*/
struct IndexBufferAttrs {
    /// number of indices in the index buffer
    int NumIndices = 0;
    /// type of indices (16-bit or 32-bit)
    IndexType::Code Type = IndexType::InvalidIndexType;
    /// buffer usage hint
    Usage::Code BufferUsage = Usage::InvalidUsage;
    /// computes the byte size of index buffer data
    int ByteSize() const;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexBufferAttrs
    @ingroup Gfx
    @brief attributes of one vertex buffer
*/
struct VertexBufferAttrs {
    /// number of vertices in the vertex buffer
    int NumVertices = 0;
    /// describes the vertex layout of a vertex in the buffer
    VertexLayout Layout;
    /// buffer usage hint
    Usage::Code BufferUsage = Usage::InvalidUsage;
    /// computes the byte size of the contained vertex buffer data
    int ByteSize() const;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxSetup
    @ingroup Gfx
    @brief Gfx module setup parameters
    
    The GfxSetup object holds the parameter used to setup the
    rendering system. Create a GfxSetup object, optionally tweak
    its values, and call the Gfx::Setup method with the
    GfxSetup object as argument.
 
    @see Gfx, DisplayAttrs
*/
class GfxSetup {
public:
    /// shortcut for windowed mode (with RGBA8, 24+8 stencil/depth, no MSAA)
    static GfxSetup Window(int width, int height, String windowTitle);
    /// shortcut for fullscreen mode (with RGBA8, 24+8 stencil/depth, no MSAA)
    static GfxSetup Fullscreen(int width, int height, String windowTitle);
    /// shortcut for windowed mode with 4xMSAA (with RGBA8, 24+8 stencil/depth)
    static GfxSetup WindowMSAA4(int width, int height, String windowTitle);
    /// shortcut for fullscreen mode with 4xMSAA (with RGBA8, 24+8 stencil/depth)
    static GfxSetup FullscreenMSAA4(int width, int height, String windowTitle);
    /// canvas width
    int Width = 640;
    /// canvas height
    int Height = 400;
    /// color pixel format
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    /// depth pixel format
    PixelFormat::Code DepthFormat = PixelFormat::DEPTHSTENCIL;
    /// MSAA samples (2, 4, 8... no MSAA: 1)
    int SampleCount = 1;
    /// windowed vs Fullscreen
    bool Windowed = true;
    /// swap interval (0 => no vsync, default is 1)
    int SwapInterval = 1;
    /// window title
    String Title = "Oryol";
    /// enable to render full-res on HighDPI displays (not supported on all platforms)
    bool HighDPI = false;
    /// default clear values (or dont care)
    PassAction DefaultPassAction;
    /// if true, ignore own size and instead track size of an HTML element (emscripten only)
    bool HtmlTrackElementSize = false;
    /// name of the HTML element to track (default: #canvas)
    String HtmlElement = "#canvas";
    /// resource pool size by resource type
    StaticArray<int,GfxResourceType::NumResourceTypes> ResourcePoolSize;
    /// resource creation throttling (max resources created async per frame)
    StaticArray<int,GfxResourceType::NumResourceTypes> ResourceThrottling;
    /// initial resource label stack capacity
    int ResourceLabelStackCapacity = 256;
    /// initial resource registry capacity
    int ResourceRegistryCapacity = 256;
    /// size of the global uniform buffer (only relevant on some platforms)
    int GlobalUniformBufferSize = GfxConfig::DefaultGlobalUniformBufferSize;
    /// max number of drawcalls per frame (only relevant on some platforms)
    int MaxDrawCallsPerFrame = GfxConfig::DefaultMaxDrawCallsPerFrame;
    /// max number of ApplyDrawState per frame (only relevant on some platforms)
    int MaxApplyDrawStatesPerFrame = GfxConfig::DefaultMaxApplyDrawStatesPerFrame;
    /// get DisplayAttrs object initialized to setup values
    DisplayAttrs GetDisplayAttrs() const;
    /// default constructor
    GfxSetup();
};

//------------------------------------------------------------------------------
/**
    @class Oryol::MeshSetup
    @ingroup Gfx
    @brief setup attributes for meshes
*/
class MeshSetup {
public:
    /// asynchronously load from file
    static MeshSetup FromFile(const class Locator& loc, Id placeholder=Id::InvalidId());
    /// setup from from data in memory
    static MeshSetup FromData(Usage::Code vertexUsage=Usage::Immutable, Usage::Code indexUsage=Usage::Immutable);
    /// setup from data in memory with blueprint
    static MeshSetup FromData(const MeshSetup& blueprint);
    /// setup empty mesh (mostly for dynamic streaming)
    static MeshSetup Empty(int numVertices, Usage::Code vertexUsage, IndexType::Code indexType=IndexType::None, int numIndices=0, Usage::Code indexUsage=Usage::InvalidUsage);
    /// setup a fullscreen quad mesh
    static MeshSetup FullScreenQuad(bool flipV=false);
    /// check if should load asynchronously
    bool ShouldSetupFromFile() const;
    /// check if should setup from data in memory
    bool ShouldSetupFromData() const;
    /// check if should setup empty mesh
    bool ShouldSetupEmpty() const;
    /// check if should setup fullscreen quad mesh
    bool ShouldSetupFullScreenQuad() const;
    /// add a primitive group (required for CreateEmpty)
    void AddPrimitiveGroup(const PrimitiveGroup& primGroup);
    /// get number of primitive groups
    int NumPrimitiveGroups() const;
    /// get primitive group at index
    const class PrimitiveGroup& PrimitiveGroup(int index) const;
    /// vertex-data usage
    Usage::Code VertexUsage = Usage::InvalidUsage;
    /// index-data usage
    Usage::Code IndexUsage = Usage::InvalidUsage;
    /// vertex layout
    VertexLayout Layout;
    /// number of vertices
    int NumVertices = 0;
    /// number of indices
    int NumIndices = 0;
    /// index type 
    IndexType::Code IndicesType = IndexType::None;
    /// flip v coordinates for fullscreen quad (so that origin is top-left)
    bool FullScreenQuadFlipV = false;
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// placeholder Id
    Id Placeholder;
    /// vertex data byte offset in data (default: 0, set to InvalidIndex if no vertex data provided)
    int VertexDataOffset = 0;
    /// index data byte offset in data (default: InvalidIndex, no index data provided)
    int IndexDataOffset = 0;
private:
    int numPrimGroups = 0;
    class PrimitiveGroup primGroups[GfxConfig::MaxNumPrimGroups];
    bool setupFromFile = false;
    bool setupFromData = false;
    bool setupEmpty = false;
    bool setupFullScreenQuad = false;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PipelineSetup
    @ingroup Gfx
    @brief setup object for pipeline resources
*/
class PipelineSetup {
public:
    /// construct from shader
    static PipelineSetup FromShader(const Id& shd);
    /// construct from vertex layout and shader
    static PipelineSetup FromLayoutAndShader(const VertexLayout& layout, const Id& shd);
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// blend state (GLES3.0 doesn't allow separate MRT blend state
    class BlendState BlendState;
    /// blend color
    glm::vec4 BlendColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    /// depth-stencil state
    class DepthStencilState DepthStencilState;
    /// rasterizer state
    class RasterizerState RasterizerState;
    /// input vertex layouts (one per mesh slot)
    StaticArray<VertexLayout, GfxConfig::MaxNumInputMeshes> Layouts;
    /// primitive type 
    PrimitiveType::Code PrimType = PrimitiveType::Triangles;
    /// shader 
    Id Shader;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PassSetup
    @ingroup Gfx
    @brief setup attributes for render pass resource
*/
class PassSetup {
public:
    /// construct from single render target textures, and option depth-stencil texture
    static PassSetup From(Id colorTexture, Id depthStencilTexture=Id::InvalidId());
    /// construct from MRT render target textures, and option depth-stencil texture
    static PassSetup From(std::initializer_list<Id> colorTextures, Id depthStencilTexture=Id::InvalidId());
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// 1..N color attachments
    struct ColorAttachment {
        Id Texture;
        uint16_t MipLevel = 0;  ///< mipmap-level
        uint16_t Slice = 0;     ///< 2D-array-slice, 3D-depth-slice or cubemap face
    };
    StaticArray<ColorAttachment, GfxConfig::MaxNumColorAttachments> ColorAttachments;
    /// optional depth-stencil attachment
    Id DepthStencilTexture;
    /// default pass action, if no PassAction provided in BeginPass
    PassAction DefaultAction;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderSetup
    @ingroup Gfx
    @brief setup class for shaders
*/
class ShaderSetup {
public:
    /// default constructor
    ShaderSetup() { }
    /// construct with resource locator
    ShaderSetup(const Locator& loc) : Locator(loc) { }
    /// the resource locator
    class Locator Locator = Locator::NonShared();
    /// set shader program from vertex- and fragment-shader sources
    void SetProgramFromSources(ShaderLang::Code slang, const String& vsSource, const String& fsSource);
    /// set shader program from precompiled shader byte code
    void SetProgramFromByteCode(ShaderLang::Code slang, const uint8_t* vsByteCode, uint32_t vsNumBytes, const uint8_t* fsByteCode, uint32_t fsNumBytes, const char* vsFunc=nullptr, const char* fsFunc=nullptr);
    /// set vertex shader input layout
    void SetInputLayout(const VertexLayout& vsInputLayout);
    /// add a uniform block
    void AddUniformBlock(const StringAtom& type, const StringAtom& name, uint32_t typeHash, uint32_t byteSize, ShaderStage::Code bindStage, int32_t bindSlot);
    /// add a texture declaration
    void AddTexture(const StringAtom& name, TextureType::Code type, ShaderStage::Code bindStage, int32_t bindSlot);
    /// get the vertex shader input layout
    const VertexLayout& InputLayout() const;
    /// get program vertex shader source (only valid if setup from sources)
    const String& VertexShaderSource(ShaderLang::Code slang) const;
    /// get program fragment shader source (only valid if setup from sources)
    const String& FragmentShaderSource(ShaderLang::Code slang) const;
    /// get program vertex shader byte code, returns nullptr if no byte code exists
    void VertexShaderByteCode(ShaderLang::Code slang, const void*& outPtr, uint32_t& outSize) const;
    /// get program fragment shader byte code, returns nullptr if no byte code exists
    void FragmentShaderByteCode(ShaderLang::Code slang, const void*& outPtr, uint32_t& outSize) const;
    /// get vertex shader name (if using metal-style shader library
    const StringAtom& VertexShaderFunc(ShaderLang::Code slang) const;
    /// get fragment shader name (if using metal-style shader library
    const StringAtom& FragmentShaderFunc(ShaderLang::Code slang) const;
    /// get number of uniform blocks
    int NumUniformBlocks() const;
    /// find uniform block index by bind stage and slot (return InvalidIndex if not found)
    int UniformBlockIndexByStageAndSlot(ShaderStage::Code bindStage, int bindSlot) const;
    /// get uniform block type at index
    const StringAtom& UniformBlockType(int index) const;
    /// get uniform block name at index
    const StringAtom& UniformBlockName(int index) const;
    /// get uniform block type hash
    uint32_t UniformBlockTypeHash(int index) const;
    /// get uniform block byte size
    uint32_t UniformBlockByteSize(int index) const;
    /// get uniform block shader stage at index
    ShaderStage::Code UniformBlockBindStage(int index) const;
    /// get uniform block bind slot at index
    int UniformBlockBindSlot(int index) const;
    /// get number of textures
    int NumTextures() const;
    /// find texture index by bind stage and slot (return InvalidIndex if not found)
    int TextureIndexByStageAndSlot(ShaderStage::Code bindStage, int bindSlot) const;
    /// get texture name at index
    const StringAtom& TexName(int index) const;
    /// get texture type at index 
    TextureType::Code TexType(int index) const;
    /// get texture bind stage
    ShaderStage::Code TexBindStage(int index) const;
    /// get texture bind slot
    int TexBindSlot(int index) const;
private:
    struct programEntry {
        StaticArray<String, ShaderLang::NumShaderLangs> vsSources;
        StaticArray<String, ShaderLang::NumShaderLangs> fsSources;
        StaticArray<StringAtom, ShaderLang::NumShaderLangs> vsFuncs;
        StaticArray<StringAtom, ShaderLang::NumShaderLangs> fsFuncs;
        struct byteCodeEntry {
            const void* ptr = nullptr;
            uint32_t size = 0;
        };
        StaticArray<byteCodeEntry, ShaderLang::NumShaderLangs> vsByteCode;
        StaticArray<byteCodeEntry, ShaderLang::NumShaderLangs> fsByteCode;
        VertexLayout vsInputLayout;
    };
    struct uniformBlockEntry {
        StringAtom type;
        StringAtom name;
        uint32_t typeHash = 0;
        uint32_t byteSize = 0;
        ShaderStage::Code bindStage = ShaderStage::InvalidShaderStage;
        int bindSlot = InvalidIndex;
    };
    struct textureEntry {
        StringAtom name;
        TextureType::Code type = TextureType::InvalidTextureType;
        ShaderStage::Code bindStage = ShaderStage::InvalidShaderStage;
        int bindSlot = InvalidIndex;
    };
    static const int MaxNumUniformBlocks = ShaderStage::NumShaderStages * GfxConfig::MaxNumUniformBlocksPerStage;
    static const int MaxNumTextures = GfxConfig::MaxNumVertexTextures + GfxConfig::MaxNumFragmentTextures;
    programEntry program;
    int numUniformBlocks = 0;
    StaticArray<uniformBlockEntry, MaxNumUniformBlocks> uniformBlocks;
    int numTextures = 0;
    StaticArray<textureEntry, MaxNumTextures> textures;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureSetup
    @ingroup Gfx
    @brief setup object for textures and render targets
*/
class TextureSetup {
public:
    /// asynchronously load from file
    static TextureSetup FromFile(const Locator& loc, Id placeholder=Id::InvalidId());
    /// asynchronously load from file
    static TextureSetup FromFile(const Locator& loc, const TextureSetup& blueprint=TextureSetup(), Id placeholder=Id::InvalidId());
    /// setup 2D texture from raw pixel data
    static TextureSetup FromPixelData2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup());
    /// setup cube texture from raw pixel data
    static TextureSetup FromPixelDataCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup());
    //// setup 3D texture from raw pixel data
    static TextureSetup FromPixelData3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup());
    /// setup array texture from raw pixel data
    static TextureSetup FromPixelDataArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup());
    /// setup empty 2D texture
    static TextureSetup Empty2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup());
    /// setup empty cube texture
    static TextureSetup EmptyCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup());
    /// setup empty 3D texture
    static TextureSetup Empty3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup());
    /// setup empty array texture
    static TextureSetup EmptyArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup());
    /// setup as 2D render target
    static TextureSetup RenderTarget2D(int w, int h, PixelFormat::Code colorFmt=PixelFormat::RGBA8, PixelFormat::Code depthFmt=PixelFormat::None);
    /// setup as cube render target
    static TextureSetup RenderTargetCube(int w, int h, PixelFormat::Code colorFmt=PixelFormat::RGBA8, PixelFormat::Code depthFmt=PixelFormat::None);
    /// setup as 3D render target
    static TextureSetup RenderTarget3D(int w, int h, int d, PixelFormat::Code colorFmt=PixelFormat::RGBA8, PixelFormat::Code depthFmt=PixelFormat::None);
    /// setup as array render target
    static TextureSetup RenderTargetArray(int w, int h, int layers, PixelFormat::Code colorFmt=PixelFormat::RGBA8, PixelFormat::Code depthFmt=PixelFormat::None);
    /// setup texture from existing native texture(s) (needs GfxFeature::NativeTexture)
    static TextureSetup FromNativeTexture(int w, int h, int numMipMaps, TextureType::Code type, PixelFormat::Code fmt, Usage::Code usage, intptr_t h0, intptr_t h1=0);
    /// return true if texture should be setup from a file
    bool ShouldSetupFromFile() const;
    /// return true if texture should be setup from raw pixel data
    bool ShouldSetupFromPixelData() const;
    /// return true if texture should be setup from native texture handles
    bool ShouldSetupFromNativeTexture() const;
    /// return true if texture should be created empty
    bool ShouldSetupEmpty() const;
    /// return true if render target has depth
    bool HasDepth() const;
    /// intended usage
    Usage::Code TextureUsage = Usage::Immutable;
    /// texture type
    TextureType::Code Type = TextureType::Texture2D;
    /// use as render target?
    bool IsRenderTarget = false;
    /// width in pixels
    int Width = 1;
    /// height in pixels
    int Height = 1;
    /// depth/layers in pixels (for 3D and Array textures)
    int Depth = 1;
    /// number of mipmaps (default is 1, only for FromPixelData)
    int NumMipMaps = 1;
    /// the color pixel format
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    /// the depth pixel format (only if render target, PixelFormat::None if render target should not have depth buffer)
    PixelFormat::Code DepthFormat = PixelFormat::None;
    /// MSAA samples (2, 4, 8... no MSAA: 1), check MSAARenderTargets feature availability!
    int SampleCount = 1;
    /// sampler state
    SamplerState Sampler;
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// resource placeholder
    Id Placeholder;
    /// optional: native texture handle (only on platforms which support GfxFeature::NativeTextures)
    static const int MaxNumNativeHandles = 2;
    StaticArray<intptr_t, MaxNumNativeHandles> NativeHandle;
    /// optional image surface offsets and sizes
    ImageDataAttrs ImageData;
    /// default constructor 
    TextureSetup();
private:
    bool setupFromFile = false;
    bool setupFromPixelData = false;
    bool setupFromNativeHandle = false;
    bool setupEmpty = false;
    bool hasMipMaps = false;
};
    
} // namespace Oryol

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
#include <functional>

namespace Oryol {

//------------------------------------------------------------------------------
/**
    @class Oryol::BufferType
    @ingroup Gfx
    @brief whether a Buffer contains vertex- or index-data
*/
struct BufferType {
    enum Code {
        VertexBuffer = 0,
        IndexBuffer,

        Num,
        Invalid
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::IndexType
    @ingroup Gfx
    @brief selects 16- or 32-bit indices
*/
struct IndexType {
    enum Code {
        None = 0,
        UInt16,
        UInt32,

        Num,
        Invalid
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
struct PixelChannel {
    typedef uint8_t Mask;
    enum Bits {
        None    = 0,

        Red     = (1<<0),
        Green   = (1<<1),
        Blue    = (1<<2),
        Alpha   = (1<<3),
        Stencil = (1<<4),
        Depth   = (1<<5),

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
struct PixelFormat {
    enum Code {
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

        Num,            ///< number of pixel formats
        Invalid,        ///< invalid pixel format value
        None = Invalid,     ///< special "none" type
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
struct PrimitiveType {
    /// primitive type enum (don't change order, append to end!)
    enum Code {
        Points = 0,
        Lines,
        LineStrip,
        Triangles,
        TriangleStrip,

        Num,
        Invalid
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
struct GfxResourceType {
    enum Code {
        Texture,            ///< a texture
        Buffer,             ///< a vertex- or index-buffer
        Shader,             ///< a shader
        Pipeline,           ///< a pipeline state object
        RenderPass,         ///< a render-pass object

        Num,
        Invalid,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderStage
    @ingroup Gfx
    @brief the shader stages (vertex shader, fragment shader)
*/
struct ShaderStage {
    enum Code {
        VS = 0,
        FS,

        Num,
        Invalid
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureFilterMode
    @ingroup Gfx
    @brief texture sampling filter mode
*/
struct TextureFilterMode {
    enum Code {
        Nearest = 0,
        Linear,
        NearestMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapNearest,
        LinearMipmapLinear,

        Num,
        Invalid,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureType
    @ingroup Gfx
    @brief texture type (2D, 3D, Cube)
*/
struct TextureType {
    enum Code {
        Texture2D = 0,
        TextureCube,
        Texture3D,
        TextureArray,

        Num,
        Invalid,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureWrapMode
    @ingroup Gfx
    @brief texture coordinate wrapping modes
*/
struct TextureWrapMode {
    enum Code {
        ClampToEdge,
        Repeat,
        MirroredRepeat,

        Num,
        Invalid,
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
struct Usage {
    enum Code {
        Immutable = 0,
        Dynamic,
        Stream,

        Num,
        Invalid,
    };
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
struct VertexFormat {
    /// format enum (don't change order, and append to end!)
    enum Code {
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

        Num,            ///< number of vertex formats
        Invalid,        ///< the invalid vertex format value
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
struct ShaderLang {
    enum Code {
        GLSL100 = 0,    ///< OpenGLES 2.0 / WebGL 1.0
        GLSL330,        ///< OpenGL 3.3
        GLSLES3,        ///< OpenGLES3
        HLSL5,          ///< D3D11 HLSL
        Metal,          ///< Metal shader language

        Num,
        Invalid
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxFeature
    @ingroup Gfx
    @brief optional rendering features
*/
struct GfxFeature {
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

        Num,
        Invalid
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Face
    @ingroup Gfx
    @brief polygon face side (front, back, both)
*/
struct Face {
    enum Code {
        Front = 0,
        Back,
        Both,

        Num,
        Invalid,
    };
    static const int NumSides = 2;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::CompareFunc
    @ingroup Gfx
    @brief comparison modes for depth and stencil state
*/
struct CompareFunc {
    enum Code {
        Never = 0,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always,

        Num,
        Invalid
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::StencilOp
    @ingroup Gfx
    @brief stencil operations
*/
struct StencilOp {
    enum Code {
        Keep = 0,
        Zero,
        Replace,
        IncrClamp,
        DecrClamp,
        Invert,
        IncrWrap,
        DecrWrap,

        Num,
        Invalid
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BlendFactor
    @ingroup Gfx
    @brief blending factors
*/
struct BlendFactor {
    enum Code {
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

        Num,
        Invalid
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BlendOperation
    @ingroup Gfx
    @brief blending operations
*/
struct BlendOperation {
    enum Code {
        Add = 0,
        Subtract,
        ReverseSubtract,

        Num,
        Invalid
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexStepFunction
    @ingroup Gfx
    @brief classify vertices in a buffer as per-vertex or per-instance data
*/
struct VertexStepFunction {
    enum Code {
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
struct PrimitiveGroup {
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
struct BlendState {
    bool BlendEnabled = false;
    BlendFactor::Code SrcFactorRGB = BlendFactor::One;
    BlendFactor::Code DstFactorRGB = BlendFactor::Zero;
    BlendOperation::Code OpRGB = BlendOperation::Add;
    BlendFactor::Code SrcFactorAlpha = BlendFactor::One;
    BlendFactor::Code DstFactorAlpha = BlendFactor::Zero;
    BlendOperation::Code OpAlpha = BlendOperation::Add;
    PixelChannel::Mask ColorWriteMask = PixelChannel::RGBA;
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    PixelFormat::Code DepthFormat = PixelFormat::DEPTHSTENCIL;
    int MRTCount = 1;
    glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::StencilState
    @ingroup Gfx
    @brief holds stencil-buffer render state for one face side
*/
struct StencilState {
    StencilOp::Code FailOp = StencilOp::Keep;
    StencilOp::Code DepthFailOp = StencilOp::Keep;
    StencilOp::Code PassOp = StencilOp::Keep;
    CompareFunc::Code CmpFunc = CompareFunc::Always;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::DepthStencilState
    @ingroup Gfx
    @brief holds the complete depth and stencil render state
*/
struct DepthStencilState {
    StencilState StencilFront;
    StencilState StencilBack;
    CompareFunc::Code DepthCmpFunc = CompareFunc::Always;
    bool DepthWriteEnabled = false;
    bool StencilEnabled = false;
    uint8_t StencilReadMask = 0xFF;
    uint8_t StencilWriteMask = 0xFF;
    uint8_t StencilRef = 0x00;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::RasterizerState
    @ingroup Gfx
    @brief rasterizer state flags
*/
struct RasterizerState {
    bool CullFaceEnabled = false;
    bool ScissorTestEnabled = false;
    bool DitherEnabled = true;
    bool AlphaToCoverageEnabled = false;
    Face::Code CullFace = Face::Back;
    int SampleCount = 1;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::SamplerState
    @ingroup Gfx
    @brief wrap texture sampler state
*/
struct SamplerState {
    TextureWrapMode::Code WrapU = TextureWrapMode::Repeat;
    TextureWrapMode::Code WrapV = TextureWrapMode::Repeat;
    TextureWrapMode::Code WrapW = TextureWrapMode::Repeat;
    TextureFilterMode::Code MagFilter = TextureFilterMode::Nearest;
    TextureFilterMode::Code MinFilter = TextureFilterMode::Nearest;
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
    - 1..4 vertex buffers
    - 0..1 index buffer
    - 0..N textures for the vertex shader stage
    - 0..N textures for the fragment shader stage
*/
struct DrawState {
    /// the pipeline state object
    Id Pipeline;
    /// vertex buffers
    StaticArray<Id, GfxConfig::MaxNumVertexBuffers> VertexBuffers;
    /// optional index buffer
    Id IndexBuffer;
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
    int NumUpdateBuffers = 0;
    int NumUpdateTextures = 0;
    int NumDraw = 0;
    int NumDrawInstanced = 0;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexLayout
    @ingroup Gfx
    @brief describes the data layout of a vertex in a vertex buffer

    FIXME: support vertex components with gaps (manually defined offset and stride)
*/
class VertexLayout {
public:
    /// a component in a vertex layout
    class Component {
    public:
        /// default constructor
        Component() {};
        /// construct from format (no attr name)
        Component(VertexFormat::Code fmt): Format(fmt) { };
        /// construct from vertex attr name and format
        Component(const StringAtom& name, VertexFormat::Code fmt): Name(name), Format(fmt) { }
        /// return true if valid (attr and format set)
        bool IsValid() const {
            return this->Format != VertexFormat::Invalid;
        }
        /// clear the component (unset attr and format)
        void Clear() {
            *this = Component();
        }
        /// get byte size of component
        int ByteSize() const {
            return VertexFormat::ByteSize(this->Format);
        }

        StringAtom Name;
        VertexFormat::Code Format = VertexFormat::Invalid;
        int Offset = 0;     // offset will be written in VertexLayout::Add
    };

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
    /// add an unnamed component
    VertexLayout& Add(VertexFormat::Code format);
    /// add a named component
    VertexLayout& Add(const StringAtom& name, VertexFormat::Code format);
    /// add multiple components via initializer list
    VertexLayout& Add(std::initializer_list<Component> l);
    /// enable layout for instancing, set StepFunction to PerInstance and StepRate to 1
    VertexLayout& EnableInstancing();
    /// get number of components
    int NumComponents() const;
    /// get component at index
    const Component& ComponentAt(int index) const;
    /// find component index by name, return InvalidIndex if not found
    int ComponentIndexByName(const StringAtom& name) const;
    /// test if the layout contains a specific vertex attribute by name
    bool Contains(const StringAtom& name) const;
    /// get byte size of vertex (aka stride)
    int ByteSize() const;
    /// get byte offset of a component
    int ComponentByteOffset(int componentIndex) const;
private:
    StaticArray<Component, GfxConfig::MaxNumVertexLayoutComponents> comps;
    int numComps = 0;
    int byteSize = 0;
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
    TextureType::Code Type = TextureType::Invalid;
    /// the pixel format of texture data
    PixelFormat::Code Format = PixelFormat::Invalid;
    /// optional sample count (only used for MSAA render target textures)
    int SampleCount = 1;
    /// texture usage hint
    Usage::Code TextureUsage = Usage::Invalid;
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
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxEvent
    @brief an event from the Gfx module
    
    Subscribe to GfxEvents with the Gfx::Subscribe() method.
*/
class GfxEvent {
public:
    /// handler function typedef
    typedef std::function<void(const GfxEvent&)> Handler;
    /// id for an event handler subscription
    typedef uint32_t HandlerId;
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
    IndexType::Code Type = IndexType::Invalid;
    /// buffer usage hint
    Usage::Code BufferUsage = Usage::Invalid;
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
    Usage::Code BufferUsage = Usage::Invalid;
    /// computes the byte size of the contained vertex buffer data
    int ByteSize() const;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxDesc
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
    StaticArray<int,GfxResourceType::Num> ResourcePoolSize;
    /// resource creation throttling (max resources created async per frame)
    StaticArray<int,GfxResourceType::Num> ResourceThrottling;
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
    @class Oryol::BufferDesc
    @ingroup Gfx
    @brief creation attributes for vertex- and index-buffers
*/
struct BufferDesc {
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// the buffer type (vertex- or index-buffer)
    BufferType::Code Type = BufferType::VertexBuffer;
    /// the buffer usage
    Usage::Code Usage = Usage::Immutable;
    /// the buffer size in bytes
    int Size = 0;
    /// optional byte-offset to init-data
    int Offset = 0;
    /// optional native 3D-API buffers
    StaticArray<intptr_t, GfxConfig::MaxInflightFrames> NativeBuffers;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BufferBuilder
    @ingroup Gfx
    @brief builder for BufferDesc object
*/
class BufferBuilder {
public:
    BufferDesc desc;
    operator BufferDesc() {
        return desc;
    }
    BufferBuilder& Locator(const class Locator& loc) {
        desc.Locator = loc; return *this;
    }
    BufferBuilder& Type(BufferType::Code t) {
        desc.Type = t; return *this;
    }
    BufferBuilder& Usage(Usage::Code u) {
        desc.Usage = u; return *this;
    }
    BufferBuilder& Size(int s) {
        desc.Size = s; return *this;
    }
    BufferBuilder& Offset(int o) {
        desc.Offset = o; return *this;
    }
    BufferBuilder& NativeBuffer(int index, intptr_t buf) {
        desc.NativeBuffers[index] = buf; return *this;
    }
};
inline BufferBuilder MakeBufferDesc() {
    return BufferBuilder();
}

//------------------------------------------------------------------------------
/**
    @class Oryol::PipelineDesc
    @ingroup Gfx
    @brief creation attribute for pipeline state objects
*/
struct PipelineDesc {
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// shader 
    Id Shader;
    /// input vertex layouts (one per vertex buffer slot)
    StaticArray<VertexLayout, GfxConfig::MaxNumVertexBuffers> Layouts;
    /// primitive type 
    PrimitiveType::Code PrimType = PrimitiveType::Triangles;
    /// index type (none, 16-bit or 32-bit)
    IndexType::Code IndexType = IndexType::None;
    /// blend state (GLES3.0 doesn't allow separate MRT blend state
    struct BlendState BlendState;
    /// depth-stencil state
    struct DepthStencilState DepthStencilState;
    /// rasterizer state
    struct RasterizerState RasterizerState;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PipelineBuilder
    @ingroup Gfx
    @brief builder for PipelineDesc objects
*/
class PipelineBuilder {
public:
    PipelineDesc desc;
    operator PipelineDesc() {
        return desc;
    }
    PipelineBuilder& Locator(const class Locator& loc) {
        desc.Locator = loc; return *this;
    }
    PipelineBuilder& Shader(const Id& shd) {
        desc.Shader = shd; return *this;
    }
    PipelineBuilder& Layout(int slotIndex, const VertexLayout& layout) {
        desc.Layouts[slotIndex] = layout; return *this;
    }
    PipelineBuilder& PrimitiveType(PrimitiveType::Code t) {
        desc.PrimType = t; return *this;
    }
    PipelineBuilder& IndexType(IndexType::Code t) {
        desc.IndexType = t; return *this;
    }
    PipelineBuilder& BlendState(const struct BlendState& bs) {
        desc.BlendState = bs; return *this;
    }
    PipelineBuilder& DepthStencilState(const struct DepthStencilState& dss) {
        desc.DepthStencilState = dss; return *this;
    }
    PipelineBuilder& RasterizerState(const struct RasterizerState& rs) {
        desc.RasterizerState = rs; return *this;
    }
};
inline PipelineBuilder MakePipelineDesc() {
    return PipelineBuilder();
}

//------------------------------------------------------------------------------
/**
    @class Oryol::PassDesc
    @ingroup Gfx
    @brief creation attributes for render pass resource
*/
class PassDesc {
public:
    /// construct from single render target textures, and option depth-stencil texture
    static PassDesc From(Id colorTexture, Id depthStencilTexture=Id::InvalidId());
    /// construct from MRT render target textures, and option depth-stencil texture
    static PassDesc From(std::initializer_list<Id> colorTextures, Id depthStencilTexture=Id::InvalidId());
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
    @class Oryol::ShaderDesc
    @ingroup Gfx
    @brief creation attributes for shaders
*/
class ShaderDesc {
public:
    /// default constructor
    ShaderDesc() { }
    /// construct with resource locator
    ShaderDesc(const Locator& loc) : Locator(loc) { }
    /// the resource locator
    class Locator Locator = Locator::NonShared();
    /// set shader program from vertex- and fragment-shader sources
    void SetProgramFromSources(ShaderLang::Code slang, const String& vsSource, const String& fsSource);
    /// set shader program from precompiled shader byte code
    void SetProgramFromByteCode(ShaderLang::Code slang, const uint8_t* vsByteCode, uint32_t vsNumBytes, const uint8_t* fsByteCode, uint32_t fsNumBytes, const char* vsFunc=nullptr, const char* fsFunc=nullptr);
    /// add a uniform block
    void AddUniformBlock(const StringAtom& type, const StringAtom& name, uint32_t byteSize, ShaderStage::Code bindStage, int32_t bindSlot);
    /// add a texture declaration
    void AddTexture(const StringAtom& name, TextureType::Code type, ShaderStage::Code bindStage, int32_t bindSlot);
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
        StaticArray<String, ShaderLang::Num> vsSources;
        StaticArray<String, ShaderLang::Num> fsSources;
        StaticArray<StringAtom, ShaderLang::Num> vsFuncs;
        StaticArray<StringAtom, ShaderLang::Num> fsFuncs;
        struct byteCodeEntry {
            const void* ptr = nullptr;
            uint32_t size = 0;
        };
        StaticArray<byteCodeEntry, ShaderLang::Num> vsByteCode;
        StaticArray<byteCodeEntry, ShaderLang::Num> fsByteCode;
    };
    struct uniformBlockEntry {
        StringAtom type;
        StringAtom name;
        uint32_t byteSize = 0;
        ShaderStage::Code bindStage = ShaderStage::Invalid;
        int bindSlot = InvalidIndex;
    };
    struct textureEntry {
        StringAtom name;
        TextureType::Code type = TextureType::Invalid;
        ShaderStage::Code bindStage = ShaderStage::Invalid;
        int bindSlot = InvalidIndex;
    };
    static const int MaxNumUniformBlocks = ShaderStage::Num * GfxConfig::MaxNumUniformBlocksPerStage;
    static const int MaxNumTextures = GfxConfig::MaxNumVertexTextures + GfxConfig::MaxNumFragmentTextures;
    programEntry program;
    int numUniformBlocks = 0;
    StaticArray<uniformBlockEntry, MaxNumUniformBlocks> uniformBlocks;
    int numTextures = 0;
    StaticArray<textureEntry, MaxNumTextures> textures;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureDesc
    @ingroup Gfx
    @brief setup object for textures and render targets
*/
class TextureDesc {
public:
    /// setup 2D texture from raw pixel data
    static TextureDesc FromPixelData2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureDesc& blueprint=TextureDesc());
    /// setup cube texture from raw pixel data
    static TextureDesc FromPixelDataCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureDesc& blueprint=TextureDesc());
    //// setup 3D texture from raw pixel data
    static TextureDesc FromPixelData3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, const TextureDesc& blueprint=TextureDesc());
    /// setup array texture from raw pixel data
    static TextureDesc FromPixelDataArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, const TextureDesc& blueprint=TextureDesc());
    /// setup empty 2D texture
    static TextureDesc Empty2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureDesc& blueprint=TextureDesc());
    /// setup empty cube texture
    static TextureDesc EmptyCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureDesc& blueprint=TextureDesc());
    /// setup empty 3D texture
    static TextureDesc Empty3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureDesc& blueprint=TextureDesc());
    /// setup empty array texture
    static TextureDesc EmptyArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureDesc& blueprint=TextureDesc());
    /// setup as 2D render target
    static TextureDesc RenderTarget2D(int w, int h, PixelFormat::Code fmt=PixelFormat::RGBA8);
    /// setup as cube render target
    static TextureDesc RenderTargetCube(int w, int h, PixelFormat::Code fmt=PixelFormat::RGBA8);
    /// setup as 3D render target
    static TextureDesc RenderTarget3D(int w, int h, int d, PixelFormat::Code fmt=PixelFormat::RGBA8);
    /// setup as array render target
    static TextureDesc RenderTargetArray(int w, int h, int layers, PixelFormat::Code fmt=PixelFormat::RGBA8);
    /// setup texture from existing native texture(s) (needs GfxFeature::NativeTexture)
    static TextureDesc FromNativeTexture(int w, int h, int numMipMaps, TextureType::Code type, PixelFormat::Code fmt, Usage::Code usage, intptr_t h0, intptr_t h1=0);
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
    /// the pixel format
    PixelFormat::Code Format = PixelFormat::RGBA8;
    /// MSAA samples (2, 4, 8... no MSAA: 1), check MSAARenderTargets feature availability!
    int SampleCount = 1;
    /// sampler state
    SamplerState Sampler;
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// resource placeholder
    Id Placeholder;
    /// optional: native textures (only on platforms which support GfxFeature::NativeTextures)
    StaticArray<intptr_t, GfxConfig::MaxInflightFrames> NativeTextures;
    /// optional image surface offsets and sizes
    ImageDataAttrs ImageData;
    /// default constructor 
    TextureDesc();
private:
    bool setupFromPixelData = false;
    bool setupFromNativeHandle = false;
    bool setupEmpty = false;
    bool hasMipMaps = false;
};
    
} // namespace Oryol

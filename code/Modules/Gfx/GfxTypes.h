#pragma once
//------------------------------------------------------------------------------
#include "Core/Types.h"
#include "Core/Assertion.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"
#include "Resource/Id.h"
#include "Resource/Locator.h"
#include "Core/Containers/StaticArray.h"
#include "Core/Containers/MemoryBuffer.h"
#include "Gfx/GfxConfig.h"
#include "glm/vec4.hpp"
#include <initializer_list>
#include <functional>

namespace Oryol {

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

        Num,
        Invalid
    };
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
        Pass,               ///< a render-pass object

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
    @class Oryol::Face
    @ingroup Gfx
    @brief polygon face side (front, back, both)
*/
struct Face {
    enum Code {
        Front = (1<<0),
        Back  = (1<<1),
        Both  = Front|Back
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
    /// return a new vertex layout object to start building via chained methods
    static VertexLayout New();
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
    /// pixel data mipmap image offsets
    StaticArray<StaticArray<int, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Offsets;
    /// pixel data mipmap image sizes
    StaticArray<StaticArray<int, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Sizes;
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
class GfxDesc {
public:
    /// shortcut for windowed mode (with RGBA8, 24+8 stencil/depth, no MSAA)
    static GfxDesc Window(int width, int height, String windowTitle);
    /// shortcut for fullscreen mode (with RGBA8, 24+8 stencil/depth, no MSAA)
    static GfxDesc Fullscreen(int width, int height, String windowTitle);
    /// shortcut for windowed mode with 4xMSAA (with RGBA8, 24+8 stencil/depth)
    static GfxDesc WindowMSAA4(int width, int height, String windowTitle);
    /// shortcut for fullscreen mode with 4xMSAA (with RGBA8, 24+8 stencil/depth)
    static GfxDesc FullscreenMSAA4(int width, int height, String windowTitle);
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
    GfxDesc();
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
    Oryol::Usage::Code Usage = Usage::Immutable;
    /// the buffer size in bytes
    int Size = 0;
    /// optional byte-offset to init-data
    int Offset = 0;
    /// optional native 3D-API buffers
    StaticArray<intptr_t, GfxConfig::MaxInflightFrames> NativeBuffers;

    /// default constructor
    BufferDesc() {
        NativeBuffers.Fill(0);
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BufferBuilder
    @ingroup Gfx
    @brief builder for BufferDesc object
*/
class BufferBuilder {
public:
    BufferDesc Desc;
    const void* ContentPtr = nullptr;
    int ContentSize = 0;
    BufferBuilder& From(const BufferDesc& desc) {
        Desc = desc; return *this;
    }
    BufferBuilder& Locator(const class Locator& loc) {
        Desc.Locator = loc; return *this;
    }
    BufferBuilder& Type(BufferType::Code t) {
        Desc.Type = t; return *this;
    }
    BufferBuilder& Usage(Usage::Code u) {
        Desc.Usage = u; return *this;
    }
    BufferBuilder& Size(int s) {
        Desc.Size = s; return *this;
    }
    BufferBuilder& Offset(int o) {
        Desc.Offset = o; return *this;
    }
    BufferBuilder& NativeBuffer(int index, intptr_t buf) {
        Desc.NativeBuffers[index] = buf; return *this;
    }
    BufferBuilder& Content(const void* ptr) {
        ContentPtr = ptr;
        ContentSize = 0;
        return *this;
    }
    BufferBuilder& Content(const MemoryBuffer& content) {
        ContentPtr = content.Data();
        ContentSize = content.Size();
        return *this;
    }
    Id Create();
};

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
    Oryol::IndexType::Code IndexType = IndexType::None;

    /// depth-stencil-state
    CompareFunc::Code DepthCmpFunc = CompareFunc::Always;
    bool DepthWriteEnabled = false;
    bool StencilEnabled = false;
    uint8_t StencilReadMask = 0xFF;
    uint8_t StencilWriteMask = 0xFF;
    uint8_t StencilRef = 0x00;
    StencilOp::Code StencilFrontFailOp = StencilOp::Keep;
    StencilOp::Code StencilFrontDepthFailOp = StencilOp::Keep;
    StencilOp::Code StencilFrontPassOp = StencilOp::Keep;
    CompareFunc::Code StencilFrontCmpFunc = CompareFunc::Always;
    StencilOp::Code StencilBackFailOp = StencilOp::Keep;
    StencilOp::Code StencilBackDepthFailOp = StencilOp::Keep;
    StencilOp::Code StencilBackPassOp = StencilOp::Keep;
    CompareFunc::Code StencilBackCmpFunc = CompareFunc::Always;

    /// blend state
    bool BlendEnabled = false;
    BlendFactor::Code BlendSrcFactorRGB = BlendFactor::One;
    BlendFactor::Code BlendDstFactorRGB = BlendFactor::Zero;
    BlendOperation::Code BlendOpRGB = BlendOperation::Add;
    BlendFactor::Code BlendSrcFactorAlpha = BlendFactor::One;
    BlendFactor::Code BlendDstFactorAlpha = BlendFactor::Zero;
    BlendOperation::Code BlendOpAlpha = BlendOperation::Add;
    PixelChannel::Mask ColorWriteMask = PixelChannel::RGBA;
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    PixelFormat::Code DepthFormat = PixelFormat::DEPTHSTENCIL;
    int MRTCount = 1;
    glm::vec4 BlendColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    /// rasterizer state
    bool CullFaceEnabled = false;
    bool AlphaToCoverageEnabled = false;
    Face::Code CullFace = Face::Back;
    int SampleCount = 1;
    float DepthBias = 0.0f;
    float DepthBiasSlopeScale = 0.0f;
    float DepthBiasClamp = 0.0f;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PipelineBuilder
    @ingroup Gfx
    @brief builder for PipelineDesc objects
*/
class PipelineBuilder {
public:
    PipelineDesc Desc;
    PipelineBuilder& From(const PipelineDesc& desc) {
        Desc = desc; return *this;
    }
    PipelineBuilder& Locator(const class Locator& loc) {
        Desc.Locator = loc; return *this;
    }
    PipelineBuilder& Shader(const Id& shd) {
        Desc.Shader = shd; return *this;
    }
    PipelineBuilder& Layout(int slotIndex, const VertexLayout& layout) {
        Desc.Layouts[slotIndex] = layout; return *this;
    }
    PipelineBuilder& PrimitiveType(PrimitiveType::Code t) {
        Desc.PrimType = t; return *this;
    }
    PipelineBuilder& IndexType(IndexType::Code t) {
        Desc.IndexType = t; return *this;
    }
    PipelineBuilder& DepthCmpFunc(CompareFunc::Code f) {
        Desc.DepthCmpFunc = f; return *this;
    }
    PipelineBuilder& DepthWriteEnabled(bool b) {
        Desc.DepthWriteEnabled = b; return *this;
    }
    PipelineBuilder& StencilEnabled(bool b) {
        Desc.StencilEnabled = b; return *this;
    }
    PipelineBuilder& StencilReadMask(uint8_t m) {
        Desc.StencilReadMask = m; return *this;
    }
    PipelineBuilder& StencilWriteMask(uint8_t m) {
        Desc.StencilWriteMask = m; return *this;
    }
    PipelineBuilder& StencilRef(uint8_t r) {
        Desc.StencilRef = r; return *this;
    }
    PipelineBuilder& StencilFailOp(Face::Code face, StencilOp::Code op) {
        if (Face::Front & face) {
            Desc.StencilFrontFailOp = op;
        }
        if (Face::Back & face) {
            Desc.StencilBackFailOp = op;
        }
        return *this;
    }
    PipelineBuilder& StencilDepthFailOp(Face::Code face, StencilOp::Code op) {
        if (Face::Front & face) {
            Desc.StencilFrontDepthFailOp = op;
        }
        if (Face::Back & face) {
            Desc.StencilBackDepthFailOp = op;
        }
        return *this;
    }
    PipelineBuilder& StencilPassOp(Face::Code face, StencilOp::Code op) {
        if (Face::Front & face) {
            Desc.StencilFrontPassOp = op;
        }
        if (Face::Back & face) {
            Desc.StencilBackPassOp = op;
        }
        return *this;
    }
    PipelineBuilder& StencilCmpFunc(Face::Code face, CompareFunc::Code fn) {
        if (Face::Front & face) {
            Desc.StencilFrontCmpFunc = fn;
        }
        if (Face::Back & face) {
            Desc.StencilBackCmpFunc = fn;
        }
        return *this;
    }
    PipelineBuilder& BlendEnabled(bool b) {
        Desc.BlendEnabled = b; return *this;
    }
    PipelineBuilder& BlendSrcFactor(BlendFactor::Code f) {
        Desc.BlendSrcFactorRGB = f;
        Desc.BlendSrcFactorAlpha = f;
        return *this;
    }
    PipelineBuilder& BlendSrcFactorRGB(BlendFactor::Code f) {
        Desc.BlendSrcFactorRGB = f; return *this;
    }
    PipelineBuilder& BlendSrcFactorAlpha(BlendFactor::Code f) {
        Desc.BlendSrcFactorAlpha = f; return *this;
    }
    PipelineBuilder& BlendDstFactor(BlendFactor::Code f) {
        Desc.BlendDstFactorRGB = f;
        Desc.BlendDstFactorAlpha = f;
        return *this;
    }
    PipelineBuilder& BlendDstFactorRGB(BlendFactor::Code f) {
        Desc.BlendDstFactorRGB = f; return *this;
    }
    PipelineBuilder& BlendDstFactorAlpha(BlendFactor::Code f) {
        Desc.BlendDstFactorAlpha = f; return *this;
    }
    PipelineBuilder& BlendOp(BlendOperation::Code op) {
        Desc.BlendOpRGB = op;
        Desc.BlendOpAlpha = op;
        return *this;
    }
    PipelineBuilder& BlendOpRGB(BlendOperation::Code op) {
        Desc.BlendOpRGB = op; return *this;
    }
    PipelineBuilder& BlendOpAlpha(BlendOperation::Code op) {
        Desc.BlendOpAlpha = op; return *this;
    }
    PipelineBuilder& ColorWriteMask(PixelChannel::Mask m) {
        Desc.ColorWriteMask = m; return *this;
    }
    PipelineBuilder& ColorFormat(PixelFormat::Code fmt) {
        Desc.ColorFormat = fmt; return *this;
    }
    PipelineBuilder& DepthFormat(PixelFormat::Code fmt) {
        Desc.DepthFormat = fmt; return *this;
    }
    PipelineBuilder& SampleCount(int c) {
        Desc.SampleCount = c; return *this;
    }
    PipelineBuilder& MRTCount(int c) {
        Desc.MRTCount = c; return *this;
    }
    PipelineBuilder& BlendColor(const glm::vec4& c) {
        Desc.BlendColor = c; return *this;
    }
    PipelineBuilder& CullFaceEnabled(bool b) {
        Desc.CullFaceEnabled = b; return *this;
    }
    PipelineBuilder& CullFace(Face::Code f) {
        Desc.CullFace = f; return *this;
    }
    PipelineBuilder& AlphaToCoverageEnabled(bool b) {
        Desc.AlphaToCoverageEnabled = b; return *this;
    }
    PipelineBuilder& DepthBias(float f) {
        Desc.DepthBias = f; return *this;
    }
    PipelineBuilder& DepthBiasSlopeScale(float f) {
        Desc.DepthBiasSlopeScale = f; return *this;
    }
    PipelineBuilder& DepthBiasClamp(float f) {
        Desc.DepthBiasClamp = f; return *this;
    }
    Id Create();
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderDesc
    @ingroup Gfx
    @brief creation attributes for shaders
*/
struct ShaderDesc {
    /// the resource locator
    class Locator Locator = Locator::NonShared();
    /// vertex shader input descriptions
    VertexLayout Layout;

    /// shader stage descriptions
    struct UniformBlockDesc {
        const char* Name = nullptr;
        const char* Type = nullptr;
        int Size = 0;
    };
    struct TextureDesc {
        const char* Name = nullptr;
        TextureType::Code Type = TextureType::Invalid;
    };
    struct StageDesc {
        const char* Source = nullptr;
        const uint8_t* ByteCode = nullptr;
        int ByteCodeSize = 0;
        const char* Entry = nullptr;
        StaticArray<UniformBlockDesc, GfxConfig::MaxNumUniformBlocksPerStage> UniformBlocks;
        StaticArray<TextureDesc, GfxConfig::MaxNumShaderTextures> Textures;
    };
    StaticArray<StageDesc, ShaderStage::Num> Stage;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderBuilder
    @ingroup Gfx
    @brief builder for ShaderDesc objects
*/
class ShaderBuilder {
public:
    ShaderDesc Desc;
    ShaderBuilder& From(const ShaderDesc& desc) {
        Desc = desc; return *this;
    }
    ShaderBuilder& Locator(const class Locator& loc) {
        Desc.Locator = loc; return *this;
    }
    ShaderBuilder& Source(ShaderStage::Code stage, const char* src) {
        Desc.Stage[stage].Source = src; return *this;
    }
    ShaderBuilder& ByteCode(ShaderStage::Code stage, const uint8_t* ptr, int size) {
        Desc.Stage[stage].ByteCode = ptr;
        Desc.Stage[stage].ByteCodeSize = size;
        return *this;
    }
    ShaderBuilder& Entry(ShaderStage::Code stage, const char* entry) {
        Desc.Stage[stage].Entry = entry; return *this;
    }
    ShaderBuilder& Attr(const StringAtom& name, VertexFormat::Code fmt) {
        Desc.Layout.Add(name, fmt); return *this;
    }
    ShaderBuilder& UniformBlock(ShaderStage::Code stage, int slot, const char* name, const char* type, int size) {
        auto& ubSlot = Desc.Stage[stage].UniformBlocks[slot];
        ubSlot.Name = name;
        ubSlot.Type = type;
        ubSlot.Size = size;
        return *this;
    }
    ShaderBuilder& Texture(ShaderStage::Code stage, int slot, const char* name, TextureType::Code type) {
        auto& texSlot = Desc.Stage[stage].Textures[slot];
        texSlot.Name = name;
        texSlot.Type = type;
        return *this;
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureDesc
    @ingroup Gfx
    @brief setup object for textures and render targets
*/
class TextureDesc {
public:
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// the texture type
    TextureType::Code Type = TextureType::Texture2D;
    /// whether the texture will be used as a render target
    bool RenderTarget = false;
    /// the texture width
    int Width = 1;
    /// the texture height
    int Height = 1;
    /// the texture depth or number of layers
    int Depth = 1;
    /// number of mipmaps in the texture
    int NumMipMaps = 1;
    /// the usage-hint of the texture
    Oryol::Usage::Code Usage = Usage::Immutable;
    /// the pixel format (can be one of the depth formats if this is a depth-render-target)
    PixelFormat::Code Format = PixelFormat::RGBA8;
    /// the sample count, for MSAA render targets
    int SampleCount = 1;
    /// magnification texture filter mode
    TextureFilterMode::Code MagFilter = TextureFilterMode::Nearest;
    /// minification texture filter mode
    TextureFilterMode::Code MinFilter = TextureFilterMode::Nearest;
    /// texture coordinate wrapping mode along U
    TextureWrapMode::Code WrapU = TextureWrapMode::Repeat;
    /// texture coordinate wrapping mode along V
    TextureWrapMode::Code WrapV = TextureWrapMode::Repeat;
    /// texture coordinate wrapping mode along W
    TextureWrapMode::Code WrapW = TextureWrapMode::Repeat;
    /// optional native textures (only on platforms which support GfxFeature::NativeTextures)
    StaticArray<intptr_t, GfxConfig::MaxInflightFrames> NativeTextures;
    /// optional image surface offsets and sizes
    ImageDataAttrs ImageData;

    /// default constructor
    TextureDesc() {
        NativeTextures.Fill(0);
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureBuilder
    @ingroup Gfx
    @brief builder for TextureDesc objects
*/
class TextureBuilder {
public:
    TextureDesc Desc;
    const void* ContentPtr = nullptr;
    int ContentSize = 0;
    static TextureBuilder New() {
        return TextureBuilder();
    }
    TextureBuilder& From(const TextureDesc& desc) {
        Desc = desc; return *this;
    }
    TextureBuilder& Locator(const class Locator& loc) {
        Desc.Locator = loc; return *this;
    }
    TextureBuilder& Type(TextureType::Code t) {
        Desc.Type = t; return *this;
    }
    TextureBuilder& RenderTarget(bool b) {
        Desc.RenderTarget = b; return *this;
    }
    TextureBuilder& Width(int w) {
        Desc.Width = w; return *this;
    }
    TextureBuilder& Height(int h) {
        Desc.Height = h; return *this;
    }
    TextureBuilder& Depth(int d) {
        Desc.Depth = d; return *this;
    }
    TextureBuilder& Layers(int l) {
        Desc.Depth = l; return *this;
    }
    TextureBuilder& NumMipMaps(int n) {
        Desc.NumMipMaps = n; return *this;
    }
    TextureBuilder& Usage(Usage::Code u) {
        Desc.Usage = u; return *this;
    }
    TextureBuilder& Format(PixelFormat::Code fmt) {
        Desc.Format = fmt; return *this;
    }
    TextureBuilder& SampleCount(int c) {
        Desc.SampleCount = c; return *this;
    }
    TextureBuilder& MagFilter(TextureFilterMode::Code f) {
        Desc.MagFilter = f; return *this;
    }
    TextureBuilder& MinFilter(TextureFilterMode::Code f) {
        Desc.MinFilter = f; return *this;
    }
    TextureBuilder& WrapU(TextureWrapMode::Code m) {
        Desc.WrapU = m; return *this;
    }
    TextureBuilder& WrapV(TextureWrapMode::Code m) {
        Desc.WrapV = m; return *this;
    }
    TextureBuilder& WrapW(TextureWrapMode::Code m) {
        Desc.WrapW = m; return *this;
    }
    TextureBuilder& NativeTexture(int index, intptr_t tex) {
        Desc.NativeTextures[index] = tex; return *this;
    }
    TextureBuilder& Content(const void* ptr, int size) {
        ContentPtr = ptr;
        ContentSize = size;
        return *this;
    }
    TextureBuilder& Content(const MemoryBuffer& content) {
        ContentPtr = content.Data();
        ContentSize = content.Size();
        return *this;
    }
    TextureBuilder& MipDataSize(int faceIndex, int mipIndex, int size) {
        Desc.ImageData.Sizes[faceIndex][mipIndex] = size;
        return *this;
    }
    TextureBuilder& MipDataOffset(int faceIndex, int mipIndex, int offset) {
        Desc.ImageData.Offsets[faceIndex][mipIndex] = offset;
        return *this;
    }
    Id Create();
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PassDesc
    @ingroup Gfx
    @brief creation attributes for render pass resource
*/
struct PassDesc {
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// a color- or depth-stencil attachment description
    struct Attachment {
        Id Texture;
        int MipLevel = 0;
        union {
            int Face = 0;
            int Layer;
            int Slice;
        };
    };
    /// color attachments (at least the first must be valid)
    StaticArray<Attachment, GfxConfig::MaxNumColorAttachments> ColorAttachments;
    /// optional depth-stencil attachment
    Attachment DepthStencilAttachment;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PassBuilder
    @ingroup Gfx
    @brief builder class for render pass creation
*/
class PassBuilder {
public:
    PassDesc Desc;
    PassBuilder& From(const PassDesc& desc) {
        Desc = desc; return *this;
    }
    PassBuilder& Locator(const class Locator& loc) {
        Desc.Locator = loc; return *this;
    }
    PassBuilder& ColorAttachment(int slotIndex, const Id& tex, int mipLevel=0, int faceLayerSlice=0) {
        auto& att = this->Desc.ColorAttachments[slotIndex];
        att.Texture = tex;
        att.MipLevel = mipLevel;
        att.Face = faceLayerSlice;
        return *this;
    }
    PassBuilder& DepthStencilAttachment(const Id& tex, int mipLevel=0, int faceLayerSlice=0) {
        auto& att = this->Desc.DepthStencilAttachment;
        att.Texture = tex;
        att.MipLevel = mipLevel;
        att.Face = faceLayerSlice;
        return *this;
    }
    Id Create();
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

} // namespace Oryol

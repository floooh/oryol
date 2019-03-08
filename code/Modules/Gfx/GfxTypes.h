#pragma once
//------------------------------------------------------------------------------
#include "Core/Types.h"
#include "Core/Assertion.h"
#include "Core/String/StringAtom.h"
#include "Resource/Id.h"
#include "Resource/Locator.h"
#include "Core/Containers/StaticArray.h"
#include "Core/Containers/MemoryBuffer.h"
#include "glm/vec4.hpp"
#include <initializer_list>
#include <functional>

namespace Oryol {

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxConfig
    @ingroup Gfx
    @brief central configuration constants of the Gfx module
*/
namespace GfxConfig {
    /// default resource pool size
    constexpr int DefaultResourcePoolSize = 128;
    /// default uniform buffer size (only relevant on some platforms)
    constexpr int DefaultGlobalUniformBufferSize = 4 * 1024 * 1024;
    /// max number of input vertex buffers
    constexpr int MaxNumVertexBuffers = 4;
    /// maximum number of textures on vertex shader stage
    constexpr int MaxNumVertexTextures = 4;
    /// maximum number of textures on fragment shader stage
    constexpr int MaxNumFragmentTextures = 12;
    /// max number of uniform blocks per stage
    constexpr int MaxNumUniformBlocksPerStage = 4;
    /// max number of textures on any stage
    constexpr int MaxNumShaderTextures = MaxNumVertexTextures>MaxNumFragmentTextures?MaxNumVertexTextures:MaxNumFragmentTextures;
    /// max number of texture faces
    constexpr int MaxNumTextureFaces = 6;
    /// max number of texture mipmaps
    constexpr int MaxNumTextureMipMaps = 12;
    /// maximum number of components in vertex layout
    constexpr int MaxNumVertexLayoutComponents = 16;
    /// maximum number of in-flight frames (not used by all platforms)
    constexpr int MaxInflightFrames = 2;
    /// maximum number of render pass color attachments
    constexpr int MaxNumColorAttachments = 4;
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
    static constexpr int NumSides = 2;
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
    /// index of first vertex or index
    int BaseElement = 0;
    /// number of vertices or indices
    int NumElements = 0;

    /// setters with chaining
    PrimitiveGroup() {};
    PrimitiveGroup(int baseElement, int numElements);
    PrimitiveGroup& SetBaseElement(int val);
    PrimitiveGroup& SetNumElements(int val);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PassAction
    @ingroup Gfx
    @brief what happens at BeginPass() 
*/
class PassAction {
public:
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

    /// default constructor
    PassAction();
    /// clear all attachments
    PassAction& Clear(float r, float g, float b, float a, float depth=1.0f, uint8_t stencil=0);
    /// clear all attachments with color as glm::vec4
    PassAction& Clear(const glm::vec4& color, float depth=1.0f, uint8_t stencil=0);
    /// clear all attachments with separate colors
    PassAction& Clear(std::initializer_list<glm::vec4> colors, float depth=1.0f, uint8_t stencil=0);
    /// load all attachments with previous content
    PassAction& Load();
    /// leave content of all attachments undefined
    PassAction& DontCare();
    /// clear all color attachments with the same color
    PassAction& ClearColor(float r, float g, float b, float a);
    /// clear all color attachments with the same color as glm::vec4
    PassAction& ClearColor(const glm::vec4& color);
    /// clear one of the color attachments
    PassAction& ClearColor(int index, float r, float g, float b, float a);
    /// clear of of the color attachments with color as glm::vec4
    PassAction& ClearColor(int index, const glm::vec4& c);
    /// load all color attachments with previous content
    PassAction& LoadColor();
    /// load one of the color attachments with previous content
    PassAction& LoadColor(int index);
    /// leave all color attachments at undefined state
    PassAction& DontCareColor();
    /// leave one of the color attachments at undefined state
    PassAction& DontCareColor(int index);

    /// clear the depth-stencil attachment
    PassAction& ClearDepthStencil(float depth=1.0f, uint8_t stencil=0);
    /// load the depth-stencil attachment with previous content
    PassAction& LoadDepthStencil();
    /// leave content of depth-stencil attachment undefined
    PassAction& DontCareDepthStencil(); 
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Bindings
    @brief describe resource bindings for the next draw call
    
    The Bindings struct describes the resource bindings for the
    next draw calls:

    - 1..4 vertex buffers (and optional offsets into those buffers)
    - 1..4 byte-offsets into vertex buffers
    - 0..1 index buffer (and optional offset)
    - optional start-offset into index buffer
    - 0..N textures for the vertex shader stage
    - 0..N textures for the fragment shader stage
*/
struct Bindings {
    /// vertex buffer slots
    StaticArray<Id, GfxConfig::MaxNumVertexBuffers> VertexBuffers;
    /// optional vertex buffer offsets
    StaticArray<int, GfxConfig::MaxNumVertexBuffers> VertexBufferOffsets;
    /// optional index buffer
    Id IndexBuffer;
    /// optional index buffer offsets
    int IndexBufferOffset = 0;
    /// vertex shader stage textures
    StaticArray<Id, GfxConfig::MaxNumVertexTextures> VSTexture;
    /// fragment shader stage textures
    StaticArray<Id, GfxConfig::MaxNumFragmentTextures> FSTexture;

    /// setters with chaining
    Bindings();
    Bindings& SetVertexBuffer(int slot, const Id& bufId);
    Bindings& SetVertexBufferOffset(int slot, int offset);
    Bindings& SetIndexBuffer(const Id& bufId);
    Bindings& SetIndexBufferOffset(int offset);
    Bindings& SetVSTexture(int slot, const Id& texId);
    Bindings& SetFSTexture(int slot, const Id& texId);
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
        Component() { }
        /// construct from format (no attr name)
        Component(VertexFormat::Code fmt): Format(fmt) { }
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
    @class Oryol::DisplayAttrs
    @ingroup Gfx
    @brief actual display attributes
    
    Holds the properties of the current display, these can be 
    different from the display setup parameters.
*/
struct DisplayAttrs {
    /// width of framebuffer associated with window
    int Width = 0;
    /// height of framebuffer associated with window
    int Height = 0;
    /// framebuffer pixel format
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    /// depth buffer pixel format (PixelFormat::None if no depth buffer)
    PixelFormat::Code DepthFormat = PixelFormat::DEPTHSTENCIL;
    /// number of multisample-anti-aliasing samples
    int SampleCount = 1;
    /// indicates windowed or fullscreen mode
    bool Windowed = true;
    /// vsync swap interval (0 means: no vsync)
    int SwapInterval = 1;
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
    enum EventType {
        DisplayModified,

        NumTypes,
        InvalidType
    };
    enum EventType Type = InvalidType;
    struct DisplayAttrs DisplayAttrs;

    /// default constructor
    GfxEvent();
    /// constructor with arguments
    GfxEvent(EventType type, const struct DisplayAttrs& attrs) : Type(type), DisplayAttrs(attrs) { }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ImageContent
    @brief describe content of image surfaces
*/
class ImageContent {
public:
    /// mipmap surface data pointers
    StaticArray<StaticArray<const void*, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Pointer;
    /// mipmap surface data sizes (in bytes)
    StaticArray<StaticArray<int, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Size;

    /// setters with chaining
    ImageContent();
    ImageContent& SetPointer(int faceIndex, int mipIndex, const void* ptr);
    ImageContent& SetSize(int faceIndex, int mipIndex, int size);
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
struct GfxDesc {
    int Width = 640;
    int Height = 400;
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    PixelFormat::Code DepthFormat = PixelFormat::DEPTHSTENCIL;
    int SampleCount = 1;
    bool Windowed = true;
    int SwapInterval = 1;
    StringAtom Title = "Oryol";
    bool HighDPI = false;
    bool HtmlTrackElementSize = false;
    StringAtom HtmlElement = "canvas";
    StaticArray<int,GfxResourceType::Num> ResourcePoolSize;
    int ResourceLabelStackCapacity = 256;
    int ResourceRegistryCapacity = 256;
    int GlobalUniformBufferSize = GfxConfig::DefaultGlobalUniformBufferSize;

    /// setters with chaining
    GfxDesc();
    GfxDesc(const GfxDesc& rhs);
    GfxDesc& SetWidth(int w);
    GfxDesc& SetHeight(int h);
    GfxDesc& SetColorFormat(PixelFormat::Code fmt);
    GfxDesc& SetDepthFormat(PixelFormat::Code fmt);
    GfxDesc& SetSampleCount(int c);
    GfxDesc& SetWindowed(bool b);
    GfxDesc& SetSwapInterval(int i);
    GfxDesc& SetTitle(const StringAtom& t);
    GfxDesc& SetHighDPI(bool b);
    GfxDesc& SetHtmlTrackElementSize(bool b);
    GfxDesc& SetHtmlElement(const StringAtom& e);
    GfxDesc& SetResourcePoolSize(GfxResourceType::Code type, int size);
    GfxDesc& SetResourceLabelStackCapacity(int c);
    GfxDesc& SetResourceRegistryCapacity(int c);
    GfxDesc& SetGlobalUniformBufferSize(int s);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BufferDesc
    @ingroup Gfx
    @brief creation attributes for vertex- and index-buffers
*/
struct BufferDesc {
    class Locator Locator = Locator::NonShared();
    BufferType::Code Type = BufferType::VertexBuffer;
    Oryol::Usage::Code Usage = Usage::Immutable;
    int Size = 0;
    const void* Content = nullptr;
    StaticArray<intptr_t, GfxConfig::MaxInflightFrames> NativeBuffers;

    /// setters with chaining
    BufferDesc();
    BufferDesc(const BufferDesc& rhs);
    BufferDesc& SetLocator(const class Locator& l);
    BufferDesc& SetType(BufferType::Code t);
    BufferDesc& SetUsage(Usage::Code u);
    BufferDesc& SetSize(int s);
    BufferDesc& SetContent(const void* c);
    BufferDesc& SetNativeBuffer(int index, intptr_t buf);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PipelineDesc
    @ingroup Gfx
    @brief creation attribute for pipeline state objects
*/
struct PipelineDesc {
    class Locator Locator = Locator::NonShared();
    Id Shader;
    StaticArray<VertexLayout, GfxConfig::MaxNumVertexBuffers> Layouts;
    PrimitiveType::Code PrimType = PrimitiveType::Triangles;
    Oryol::IndexType::Code IndexType = IndexType::None;
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
    bool CullFaceEnabled = false;
    bool AlphaToCoverageEnabled = false;
    Face::Code CullFace = Face::Back;
    int SampleCount = 1;
    float DepthBias = 0.0f;
    float DepthBiasSlopeScale = 0.0f;
    float DepthBiasClamp = 0.0f;

    /// setters with chaining
    PipelineDesc() { };
    PipelineDesc(const PipelineDesc& rhs);
    PipelineDesc& SetLocator(const class Locator& loc);
    PipelineDesc& SetShader(const Id& shd);
    PipelineDesc& SetLayout(int slotIndex, const VertexLayout& layout);
    PipelineDesc& SetPrimitiveType(PrimitiveType::Code t);
    PipelineDesc& SetIndexType(IndexType::Code t);
    PipelineDesc& SetDepthCmpFunc(CompareFunc::Code f);
    PipelineDesc& SetDepthWriteEnabled(bool b);
    PipelineDesc& SetStencilEnabled(bool b);
    PipelineDesc& SetStencilReadMask(uint8_t m);
    PipelineDesc& SetStencilWriteMask(uint8_t m);
    PipelineDesc& SetStencilRef(uint8_t r);
    PipelineDesc& SetStencilFailOp(Face::Code face, StencilOp::Code op);
    PipelineDesc& SetStencilDepthFailOp(Face::Code face, StencilOp::Code op);
    PipelineDesc& SetStencilPassOp(Face::Code face, StencilOp::Code op);
    PipelineDesc& SetStencilCmpFunc(Face::Code face, CompareFunc::Code fn);
    PipelineDesc& SetBlendEnabled(bool b);
    PipelineDesc& SetBlendSrcFactor(BlendFactor::Code f);
    PipelineDesc& SetBlendSrcFactorRGB(BlendFactor::Code f);
    PipelineDesc& SetBlendSrcFactorAlpha(BlendFactor::Code f);
    PipelineDesc& SetBlendDstFactor(BlendFactor::Code f);
    PipelineDesc& SetBlendDstFactorRGB(BlendFactor::Code f);
    PipelineDesc& SetBlendDstFactorAlpha(BlendFactor::Code f);
    PipelineDesc& SetBlendOp(BlendOperation::Code op);
    PipelineDesc& SetBlendOpRGB(BlendOperation::Code op);
    PipelineDesc& SetBlendOpAlpha(BlendOperation::Code op);
    PipelineDesc& SetColorWriteMask(PixelChannel::Mask m);
    PipelineDesc& SetColorFormat(PixelFormat::Code fmt);
    PipelineDesc& SetDepthFormat(PixelFormat::Code fmt);
    PipelineDesc& SetSampleCount(int c);
    PipelineDesc& SetMRTCount(int c);
    PipelineDesc& SetBlendColor(const glm::vec4& c);
    PipelineDesc& SetCullFaceEnabled(bool b);
    PipelineDesc& SetCullFace(Face::Code f);
    PipelineDesc& SetAlphaToCoverageEnabled(bool b);
    PipelineDesc& SetDepthBias(float f);
    PipelineDesc& SetDepthBiasSlopeScale(float f);
    PipelineDesc& SetDepthBiasClamp(float f);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderDesc
    @ingroup Gfx
    @brief creation attributes for shaders
*/
struct ShaderDesc {
    class Locator Locator = Locator::NonShared();
    VertexLayout Layout;
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

    /// setters with chaining
    ShaderDesc() { };
    ShaderDesc(const ShaderDesc& rhs);
    ShaderDesc& SetLocator(const class Locator& loc);
    ShaderDesc& SetSource(ShaderStage::Code stg, const char* src);
    ShaderDesc& SetByteCode(ShaderStage::Code stg, const uint8_t* ptr, int size);
    ShaderDesc& SetEntry(ShaderStage::Code stg, const char* entry);
    ShaderDesc& SetAttr(const StringAtom& name, VertexFormat::Code fmt);
    ShaderDesc& SetUniformBlock(ShaderStage::Code stg, int slot, const char* name, const char* type, int size);
    ShaderDesc& SetTexture(ShaderStage::Code stg, int slot, const char* name, TextureType::Code type);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureDesc
    @ingroup Gfx
    @brief setup object for textures and render targets
*/
struct TextureDesc {
    class Locator Locator = Locator::NonShared();
    TextureType::Code Type = TextureType::Texture2D;
    bool RenderTarget = false;
    int Width = 1;
    int Height = 1;
    int Depth = 1;
    int NumMipMaps = 1;
    Oryol::Usage::Code Usage = Usage::Immutable;
    PixelFormat::Code Format = PixelFormat::RGBA8;
    int SampleCount = 1;
    TextureFilterMode::Code MagFilter = TextureFilterMode::Nearest;
    TextureFilterMode::Code MinFilter = TextureFilterMode::Nearest;
    TextureWrapMode::Code WrapU = TextureWrapMode::Repeat;
    TextureWrapMode::Code WrapV = TextureWrapMode::Repeat;
    TextureWrapMode::Code WrapW = TextureWrapMode::Repeat;
    StaticArray<intptr_t, GfxConfig::MaxInflightFrames> NativeTextures;
    ImageContent Content;

    /// setters with chaining
    TextureDesc();
    TextureDesc(const TextureDesc& rhs);
    TextureDesc& SetLocator(const class Locator& loc);
    TextureDesc& SetType(TextureType::Code t);
    TextureDesc& SetRenderTarget(bool b);
    TextureDesc& SetWidth(int w);
    TextureDesc& SetHeight(int h);
    TextureDesc& SetDepth(int d);
    TextureDesc& SetLayers(int l);
    TextureDesc& SetNumMipMaps(int n);
    TextureDesc& SetUsage(Usage::Code u);
    TextureDesc& SetFormat(PixelFormat::Code fmt);
    TextureDesc& SetSampleCount(int c);
    TextureDesc& SetMagFilter(TextureFilterMode::Code f);
    TextureDesc& SetMinFilter(TextureFilterMode::Code f);
    TextureDesc& SetWrapU(TextureWrapMode::Code m);
    TextureDesc& SetWrapV(TextureWrapMode::Code m);
    TextureDesc& SetWrapW(TextureWrapMode::Code m);
    TextureDesc& SetNativeTexture(int index, intptr_t tex);
    TextureDesc& SetMipSize(int faceIndex, int mipIndex, int size);
    TextureDesc& SetMipContent(int faceIndex, int mipIndex, const void* ptr);
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PassDesc
    @ingroup Gfx
    @brief creation attributes for render pass resource
*/
struct PassDesc {
    class Locator Locator = Locator::NonShared();
    struct Attachment {
        Id Texture;
        int MipLevel = 0;
        union {
            int Face = 0;
            int Layer;
            int Slice;
        };
    };
    StaticArray<Attachment, GfxConfig::MaxNumColorAttachments> ColorAttachments;
    Attachment DepthStencilAttachment;

    /// setters with chaining
    PassDesc() { };
    PassDesc(const PassDesc& rhs);
    PassDesc& SetLocator(const class Locator& loc);
    PassDesc& SetColorAttachment(int slotIndex, const Id& tex, int mipLevel=0, int faceLayerSlice=0);
    PassDesc& SetDepthStencilAttachment(const Id& tex, int mipLevel=0, int faceLayerSlice=0);
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
    int NumApplyPipeline = 0;
    int NumApplyBindings = 0;
    int NumApplyUniforms = 0;
    int NumUpdateBuffer = 0;
    int NumUpdateTexture = 0;
    int NumDraw = 0;
    int NumDrawInstanced = 0;
};

} // namespace Oryol

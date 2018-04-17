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
class GfxConfig {
public:
    /// default resource pool size
    static const int DefaultResourcePoolSize = 128;
    /// default uniform buffer size (only relevant on some platforms)
    static const int DefaultGlobalUniformBufferSize = 4 * 1024 * 1024;
    /// default maximum number of draw-calls per frame (only relevant on some platforms)
    static const int DefaultMaxDrawCallsPerFrame = (1<<16);
    /// default maximum number of Gfx::ApplyDrawState per frame (only relevant on some platforms)
    static const int DefaultMaxApplyDrawStatesPerFrame = 4096;
    /// max number of input vertex buffers
    static const int MaxNumVertexBuffers = 4;
    /// maximum number of textures on vertex shader stage
    static const int MaxNumVertexTextures = 4;
    /// maximum number of textures on fragment shader stage
    static const int MaxNumFragmentTextures = 12;
    /// max number of uniform blocks per stage
    static const int MaxNumUniformBlocksPerStage = 4;
    /// max number of textures on any stage
    static const int MaxNumShaderTextures = MaxNumVertexTextures>MaxNumFragmentTextures?MaxNumVertexTextures:MaxNumFragmentTextures;
    /// max number of texture faces
    static const int MaxNumTextureFaces = 6;
    /// max number of texture array slices
    static const int MaxNumTextureArraySlices = 128;
    /// max number of texture mipmaps
    static const int MaxNumTextureMipMaps = 12;
    /// maximum number of components in vertex layout
    static const int MaxNumVertexLayoutComponents = 16;
    /// maximum number of in-flight frames (not used by all platforms)
    static const int MaxInflightFrames = 2;
    /// maximum number of render pass color attachments
    static const int MaxNumColorAttachments = 4;
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

    /// return a default-initialized PassAction object
    static PassAction New();

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

    /// default constructor
    PassAction();
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
    @class Oryol::ImageContent
    @brief describe content of image surfaces
*/
class ImageContent {
public:
    /// constructor
    ImageContent();
    /// pixel data mipmap image offsets
    StaticArray<StaticArray<const void*, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Pointer;
    /// pixel data mipmap image sizes
    StaticArray<StaticArray<int, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Size;
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
    GfxDesc() {
        for (int i = 0; i < GfxResourceType::Num; i++) {
            resourcePoolSize[i] = GfxConfig::DefaultResourcePoolSize;
        }
    }
    GfxDesc(const GfxDesc& rhs) {
        *this = rhs;
    }
    GfxDesc& Width(int w) {
        width = w; return *this; 
    }
    int Width() const {
        return width;
    }
    GfxDesc& Height(int h) {
        height = h; return *this;
    }
    int Height() const {
        return height;
    }
    GfxDesc& ColorFormat(PixelFormat::Code fmt) {
        colorFormat = fmt; return *this;
    }
    PixelFormat::Code ColorFormat() const {
        return colorFormat;
    }
    GfxDesc& DepthFormat(PixelFormat::Code fmt) {
        depthFormat = fmt; return *this;
    }
    PixelFormat::Code DepthFormat() const {
        return depthFormat;
    }
    GfxDesc& SampleCount(int c) {
        sampleCount = c; return *this;
    }
    int SampleCount() const {
        return sampleCount;
    }
    GfxDesc& Windowed(bool b) {
        windowed = b; return *this;
    }
    bool Windowed() const {
        return windowed;
    }
    GfxDesc& SwapInterval(int i) {
        swapInterval = i; return *this;
    }
    int SwapInterval() const {
        return swapInterval;
    }
    GfxDesc& Title(const StringAtom& t) {
        title = t; return *this;
    }
    const StringAtom& Title() const {
        return title;
    }
    GfxDesc& HighDPI(bool b) {
        highDPI = b; return *this;
    }
    bool HighDPI() const {
        return highDPI;
    }
    GfxDesc& HtmlTrackElementSize(bool b) {
        htmlTrackElementSize = b; return *this;
    }
    bool HtmlTrackElementSize() const {
        return htmlTrackElementSize;
    }
    GfxDesc& HtmlElement(const StringAtom& e) {
        htmlElement = e; return *this;
    }
    const StringAtom& HtmlElement() const {
        return htmlElement;
    }
    GfxDesc& ResourcePoolSize(GfxResourceType::Code type, int size) {
        resourcePoolSize[type] = size; return *this;
    }
    int ResourcePoolSize(GfxResourceType::Code type) const {
        return resourcePoolSize[type];
    }
    GfxDesc& ResourceLabelStackCapacity(int c) {
        resourceLabelStackCapacity = c; return *this;
    }
    int ResourceLabelStackCapacity() const {
        return resourceLabelStackCapacity;
    }
    GfxDesc& ResourceRegistryCapacity(int c) {
        resourceRegistryCapacity = c; return *this;
    }
    int ResourceRegistryCapacity() const {
        return resourceRegistryCapacity;
    }
    GfxDesc& GlobalUniformBufferSize(int s) {
        globalUniformBufferSize = s; return *this;
    }
    int GlobalUniformBufferSize() const {
        return globalUniformBufferSize;
    }

    int width = 640;
    int height = 400;
    PixelFormat::Code colorFormat = PixelFormat::RGBA8;
    PixelFormat::Code depthFormat = PixelFormat::DEPTHSTENCIL;
    int sampleCount = 1;
    bool windowed = true;
    int swapInterval = 1;
    StringAtom title = "Oryol";
    bool highDPI = false;
    bool htmlTrackElementSize = false;
    StringAtom htmlElement = "#canvas";
    StaticArray<int,GfxResourceType::Num> resourcePoolSize;
    int resourceLabelStackCapacity = 256;
    int resourceRegistryCapacity = 256;
    int globalUniformBufferSize = GfxConfig::DefaultGlobalUniformBufferSize;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BufferDesc
    @ingroup Gfx
    @brief creation attributes for vertex- and index-buffers
*/
struct BufferDesc {
    BufferDesc() {
        nativeBuffers.Fill(0);
    }
    BufferDesc(const BufferDesc& rhs) {
        *this = rhs;
    }
    BufferDesc& Locator(const class Locator& l) {
        locator = l; return *this;
    }
    const class Locator& Locator() const {
        return locator;
    }
    BufferDesc& Type(BufferType::Code t) {
        type = t; return *this;
    }
    BufferType::Code Type() const {
        return type;
    }
    BufferDesc& Usage(Usage::Code u) {
        usage = u; return *this;
    }
    Usage::Code Usage() const {
        return usage;
    }
    BufferDesc& Size(int s) {
        size = s; return *this;
    }
    int Size() const {
        return size;
    }
    BufferDesc& Content(const void* c) {
        content = c; return *this;
    }
    const void* Content() const {
        return content;
    }
    BufferDesc& NativeBuffer(int index, intptr_t buf) {
        nativeBuffers[index] = buf; return *this;
    }
    intptr_t NativeBuffer(int index) const {
        return nativeBuffers[index];
    }

    class Locator locator = Locator::NonShared();
    BufferType::Code type = BufferType::VertexBuffer;
    Oryol::Usage::Code usage = Usage::Immutable;
    int size = 0;
    const void* content = nullptr;
    StaticArray<intptr_t, GfxConfig::MaxInflightFrames> nativeBuffers;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PipelineDesc
    @ingroup Gfx
    @brief creation attribute for pipeline state objects
*/
struct PipelineDesc {
    PipelineDesc() { };
    PipelineDesc(const PipelineDesc& rhs) {
        *this = rhs;
    }
    PipelineDesc& Locator(const class Locator& loc) {
        locator = loc; return *this;
    }
    const class Locator& Locator() const {
        return locator;
    }
    PipelineDesc& Shader(const Id& shd) {
        shader = shd; return *this;
    }
    const Id& Shader() const {
        return shader;
    }
    PipelineDesc& Layout(int slotIndex, const VertexLayout& layout) {
        layouts[slotIndex] = layout; return *this;
    }
    const VertexLayout& Layout(int slotIndex) const {
        return layouts[slotIndex];
    }
    PipelineDesc& PrimitiveType(PrimitiveType::Code t) {
        primType = t; return *this;
    }
    PrimitiveType::Code PrimitiveType() const {
        return primType;
    }
    PipelineDesc& IndexType(IndexType::Code t) {
        indexType = t; return *this;
    }
    IndexType::Code IndexType() const {
        return indexType;
    }
    PipelineDesc& DepthCmpFunc(CompareFunc::Code f) {
        depthCmpFunc = f; return *this;
    }
    CompareFunc::Code DepthCmpFunc() const {
        return depthCmpFunc;
    }
    PipelineDesc& DepthWriteEnabled(bool b) {
        depthWriteEnabled = b; return *this;
    }
    bool DepthWriteEnabled() const {
        return depthWriteEnabled;
    }
    PipelineDesc& StencilEnabled(bool b) {
        stencilEnabled = b; return *this;
    }
    bool StencilEnabled() const {
        return stencilEnabled;
    }
    PipelineDesc& StencilReadMask(uint8_t m) {
        stencilReadMask = m; return *this;
    }
    uint8_t StencilReadMask() const {
        return stencilReadMask;
    }
    PipelineDesc& StencilWriteMask(uint8_t m) {
        stencilWriteMask = m; return *this;
    }
    uint8_t StencilWriteMask() const {
        return stencilWriteMask;
    }
    PipelineDesc& StencilRef(uint8_t r) {
        stencilRef = r; return *this;
    }
    uint8_t StencilRef() const {
        return stencilRef;
    }
    PipelineDesc& StencilFailOp(Face::Code face, StencilOp::Code op) {
        if (Face::Front & face) {
            stencilFrontFailOp = op;
        }
        if (Face::Back & face) {
            stencilBackFailOp = op;
        }
        return *this;
    }
    StencilOp::Code StencilFailOp(Face::Code face) const {
        if (Face::Front & face) {
            return stencilFrontFailOp;
        }
        else {
            return stencilBackFailOp;
        }
    }
    PipelineDesc& StencilDepthFailOp(Face::Code face, StencilOp::Code op) {
        if (Face::Front & face) {
            stencilFrontDepthFailOp = op;
        }
        if (Face::Back & face) {
            stencilBackDepthFailOp = op;
        }
        return *this;
    }
    StencilOp::Code StencilDepthFailOp(Face::Code face) {
        if (Face::Front & face) {
            return stencilFrontDepthFailOp;
        }
        else {
            return stencilBackDepthFailOp;
        }
    }
    PipelineDesc& StencilPassOp(Face::Code face, StencilOp::Code op) {
        if (Face::Front & face) {
            stencilFrontPassOp = op;
        }
        if (Face::Back & face) {
            stencilBackPassOp = op;
        }
        return *this;
    }
    StencilOp::Code StencilPassOp(Face::Code face) {
        if (Face::Front & face) {
            return stencilFrontPassOp;
        }
        else {
            return stencilBackPassOp;
        }
    }
    PipelineDesc& StencilCmpFunc(Face::Code face, CompareFunc::Code fn) {
        if (Face::Front & face) {
            stencilFrontCmpFunc = fn;
        }
        if (Face::Back & face) {
            stencilBackCmpFunc = fn;
        }
        return *this;
    }
    CompareFunc::Code StencilCmpFunc(Face::Code face) {
        if (Face::Front & face) {
            return stencilFrontCmpFunc;
        }
        else {
            return stencilBackCmpFunc;
        }
    }
    PipelineDesc& BlendEnabled(bool b) {
        blendEnabled = b; return *this;
    }
    bool BlendEnabled() const {
        return blendEnabled;
    }
    PipelineDesc& BlendSrcFactor(BlendFactor::Code f) {
        blendSrcFactorRGB = f;
        blendSrcFactorAlpha = f;
        return *this;
    }
    PipelineDesc& BlendSrcFactorRGB(BlendFactor::Code f) {
        blendSrcFactorRGB = f; return *this;
    }
    BlendFactor::Code BlendSrcFactorRGB() const {
        return blendSrcFactorRGB;
    }
    PipelineDesc& BlendSrcFactorAlpha(BlendFactor::Code f) {
        blendSrcFactorAlpha = f; return *this;
    }
    BlendFactor::Code BlendSrcFactorAlpha() const {
        return blendSrcFactorAlpha;
    }
    PipelineDesc& BlendDstFactor(BlendFactor::Code f) {
        blendDstFactorRGB = f;
        blendDstFactorAlpha = f;
        return *this;
    }
    PipelineDesc& BlendDstFactorRGB(BlendFactor::Code f) {
        blendDstFactorRGB = f; return *this;
    }
    BlendFactor::Code BlendDstFactorRGB() const {
        return blendDstFactorRGB;
    }
    PipelineDesc& BlendDstFactorAlpha(BlendFactor::Code f) {
        blendDstFactorAlpha = f; return *this;
    }
    BlendFactor::Code BlendDstFactorAlpha() const {
        return blendDstFactorAlpha;
    }
    PipelineDesc& BlendOp(BlendOperation::Code op) {
        blendOpRGB = op;
        blendOpAlpha = op;
        return *this;
    }
    PipelineDesc& BlendOpRGB(BlendOperation::Code op) {
        blendOpRGB = op; return *this;
    }
    BlendOperation::Code BlendOpRGB() const {
        return blendOpRGB;
    }
    PipelineDesc& BlendOpAlpha(BlendOperation::Code op) {
        blendOpAlpha = op; return *this;
    }
    BlendOperation::Code BlendOpAlpha() const {
        return blendOpAlpha;
    }
    PipelineDesc& ColorWriteMask(PixelChannel::Mask m) {
        colorWriteMask = m; return *this;
    }
    PixelChannel::Mask ColorWriteMask() const {
        return colorWriteMask;
    }
    PipelineDesc& ColorFormat(PixelFormat::Code fmt) {
        colorFormat = fmt; return *this;
    }
    PixelFormat::Code ColorFormat() const {
        return colorFormat;
    }
    PipelineDesc& DepthFormat(PixelFormat::Code fmt) {
        depthFormat = fmt; return *this;
    }
    PixelFormat::Code DepthFormat() const {
        return depthFormat;
    }
    PipelineDesc& SampleCount(int c) {
        sampleCount = c; return *this;
    }
    int SampleCount() const {
        return sampleCount;
    }
    PipelineDesc& MRTCount(int c) {
        mrtCount = c; return *this;
    }
    int MRTCount() const {
        return mrtCount;
    }
    PipelineDesc& BlendColor(const glm::vec4& c) {
        blendColor = c; return *this;
    }
    const glm::vec4& BlendColor() const {
        return blendColor;
    }
    PipelineDesc& CullFaceEnabled(bool b) {
        cullFaceEnabled = b; return *this;
    }
    bool CullFaceEnabled() const {
        return cullFaceEnabled;
    }
    PipelineDesc& CullFace(Face::Code f) {
        cullFace = f; return *this;
    }
    Face::Code CullFace() const {
        return cullFace;
    }
    PipelineDesc& AlphaToCoverageEnabled(bool b) {
        alphaToCoverageEnabled = b; return *this;
    }
    bool AlphaToCoverageEnabled() const {
        return alphaToCoverageEnabled;
    }
    PipelineDesc& DepthBias(float f) {
        depthBias = f; return *this;
    }
    float DepthBias() const {
        return depthBias;
    }
    PipelineDesc& DepthBiasSlopeScale(float f) {
        depthBiasSlopeScale = f; return *this;
    }
    float DepthBiasSlopeScale() const {
        return depthBiasSlopeScale;
    }
    PipelineDesc& DepthBiasClamp(float f) {
        depthBiasClamp = f; return *this;
    }
    float DepthBiasClamp() const {
        return depthBiasClamp;
    }

    class Locator locator = Locator::NonShared();
    Id shader;
    StaticArray<VertexLayout, GfxConfig::MaxNumVertexBuffers> layouts;
    PrimitiveType::Code primType = PrimitiveType::Triangles;
    Oryol::IndexType::Code indexType = IndexType::None;

    CompareFunc::Code depthCmpFunc = CompareFunc::Always;
    bool depthWriteEnabled = false;
    bool stencilEnabled = false;
    uint8_t stencilReadMask = 0xFF;
    uint8_t stencilWriteMask = 0xFF;
    uint8_t stencilRef = 0x00;
    StencilOp::Code stencilFrontFailOp = StencilOp::Keep;
    StencilOp::Code stencilFrontDepthFailOp = StencilOp::Keep;
    StencilOp::Code stencilFrontPassOp = StencilOp::Keep;
    CompareFunc::Code stencilFrontCmpFunc = CompareFunc::Always;
    StencilOp::Code stencilBackFailOp = StencilOp::Keep;
    StencilOp::Code stencilBackDepthFailOp = StencilOp::Keep;
    StencilOp::Code stencilBackPassOp = StencilOp::Keep;
    CompareFunc::Code stencilBackCmpFunc = CompareFunc::Always;

    bool blendEnabled = false;
    BlendFactor::Code blendSrcFactorRGB = BlendFactor::One;
    BlendFactor::Code blendDstFactorRGB = BlendFactor::Zero;
    BlendOperation::Code blendOpRGB = BlendOperation::Add;
    BlendFactor::Code blendSrcFactorAlpha = BlendFactor::One;
    BlendFactor::Code blendDstFactorAlpha = BlendFactor::Zero;
    BlendOperation::Code blendOpAlpha = BlendOperation::Add;
    PixelChannel::Mask colorWriteMask = PixelChannel::RGBA;
    PixelFormat::Code colorFormat = PixelFormat::RGBA8;
    PixelFormat::Code depthFormat = PixelFormat::DEPTHSTENCIL;
    int mrtCount = 1;
    glm::vec4 blendColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    bool cullFaceEnabled = false;
    bool alphaToCoverageEnabled = false;
    Face::Code cullFace = Face::Back;
    int sampleCount = 1;
    float depthBias = 0.0f;
    float depthBiasSlopeScale = 0.0f;
    float depthBiasClamp = 0.0f;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderDesc
    @ingroup Gfx
    @brief creation attributes for shaders
*/
struct ShaderDesc {
    ShaderDesc() { };
    ShaderDesc(const ShaderDesc& rhs) {
        *this = rhs;
    }
    ShaderDesc& Locator(const class Locator& loc) {
        locator = loc; return *this;
    }
    const class Locator& Locator() const {
        return locator;
    }
    ShaderDesc& Source(ShaderStage::Code stg, const char* src) {
        stage[stg].Source = src; return *this;
    }
    const char* Source(ShaderStage::Code stg) {
        return stage[stg].Source;
    }
    ShaderDesc& ByteCode(ShaderStage::Code stg, const uint8_t* ptr, int size) {
        stage[stg].ByteCode = ptr;
        stage[stg].ByteCodeSize = size;
        return *this;
    }
    const uint8_t* ByteCodePtr(ShaderStage::Code stg) const {
        return stage[stg].ByteCode;
    }
    int ByteCodeSize(ShaderStage::Code stg) const {
        return stage[stg].ByteCodeSize;
    }
    ShaderDesc& Entry(ShaderStage::Code stg, const char* entry) {
        stage[stg].Entry = entry; return *this;
    }
    const char* Entry(ShaderStage::Code stg) const {
        return stage[stg].Entry;
    }
    ShaderDesc& Attr(const StringAtom& name, VertexFormat::Code fmt) {
        layout.Add(name, fmt); return *this;
    }
    const VertexLayout& Layout() const {
        return layout;
    }
    ShaderDesc& UniformBlock(ShaderStage::Code stg, int slot, const char* name, const char* type, int size) {
        auto& ubSlot = stage[stg].UniformBlocks[slot];
        ubSlot.Name = name;
        ubSlot.Type = type;
        ubSlot.Size = size;
        return *this;
    }
    ShaderDesc& Texture(ShaderStage::Code stg, int slot, const char* name, TextureType::Code type) {
        auto& texSlot = stage[stg].Textures[slot];
        texSlot.Name = name;
        texSlot.Type = type;
        return *this;
    }

    class Locator locator = Locator::NonShared();
    VertexLayout layout;
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
    StaticArray<StageDesc, ShaderStage::Num> stage;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureDesc
    @ingroup Gfx
    @brief setup object for textures and render targets
*/
struct TextureDesc {
    TextureDesc() {
        nativeTextures.Fill(0);
    }
    TextureDesc(const TextureDesc& rhs) {
        *this = rhs;
    }
    TextureDesc& Locator(const class Locator& loc) {
        locator = loc; return *this;
    }
    const class Locator& Locator() const {
        return locator;
    }
    TextureDesc& Type(TextureType::Code t) {
        type = t; return *this;
    }
    TextureType::Code Type() const {
        return type;
    }
    TextureDesc& RenderTarget(bool b) {
        renderTarget = b; return *this;
    }
    bool RenderTarget() const {
        return renderTarget;
    }
    TextureDesc& Width(int w) {
        width = w; return *this;
    }
    int Width() const {
        return width;
    }
    TextureDesc& Height(int h) {
        height = h; return *this;
    }
    int Height() const {
        return height;
    }
    TextureDesc& Depth(int d) {
        depth = d; return *this;
    }
    int Depth() const {
        return depth;
    }
    TextureDesc& Layers(int l) {
        depth = l; return *this;
    }
    int Layers() const {
        return depth;
    }
    TextureDesc& NumMipMaps(int n) {
        numMipMaps = n; return *this;
    }
    int NumMipMaps() const {
        return numMipMaps;
    }
    TextureDesc& Usage(Usage::Code u) {
        usage = u; return *this;
    }
    Usage::Code Usage() const {
        return usage;
    }
    TextureDesc& Format(PixelFormat::Code fmt) {
        format = fmt; return *this;
    }
    PixelFormat::Code Format() const {
        return format;
    }
    TextureDesc& SampleCount(int c) {
        sampleCount = c; return *this;
    }
    int SampleCount() const {
        return sampleCount;
    }
    TextureDesc& MagFilter(TextureFilterMode::Code f) {
        magFilter = f; return *this;
    }
    TextureFilterMode::Code MagFilter() const {
        return magFilter;
    }
    TextureDesc& MinFilter(TextureFilterMode::Code f) {
        minFilter = f; return *this;
    }
    TextureFilterMode::Code MinFilter() const {
        return minFilter;
    }
    TextureDesc& WrapU(TextureWrapMode::Code m) {
        wrapU = m; return *this;
    }
    TextureWrapMode::Code WrapU() const {
        return wrapU;
    }
    TextureDesc& WrapV(TextureWrapMode::Code m) {
        wrapV = m; return *this;
    }
    TextureWrapMode::Code WrapV() const {
        return wrapV;
    }
    TextureDesc& WrapW(TextureWrapMode::Code m) {
        wrapW = m; return *this;
    }
    TextureWrapMode::Code WrapW() const {
        return wrapW;
    }
    TextureDesc& NativeTexture(int index, intptr_t tex) {
        nativeTextures[index] = tex; return *this;
    }
    intptr_t NativeTexture(int index) const {
        return nativeTextures[index];
    }
    TextureDesc& MipSize(int faceIndex, int mipIndex, int size) {
        content.Size[faceIndex][mipIndex] = size;
        return *this;
    }
    int MipSize(int faceIndex, int mipIndex) const {
        return content.Size[faceIndex][mipIndex];
    }
    TextureDesc& MipContent(int faceIndex, int mipIndex, const void* ptr) {
        content.Pointer[faceIndex][mipIndex] = ptr;
        return *this;
    }
    const void* MipContent(int faceIndex, int mipIndex) const {
        return content.Pointer[faceIndex][mipIndex];
    }

    class Locator locator = Locator::NonShared();
    TextureType::Code type = TextureType::Texture2D;
    bool renderTarget = false;
    int width = 1;
    int height = 1;
    int depth = 1;
    int numMipMaps = 1;
    Oryol::Usage::Code usage = Usage::Immutable;
    PixelFormat::Code format = PixelFormat::RGBA8;
    int sampleCount = 1;
    TextureFilterMode::Code magFilter = TextureFilterMode::Nearest;
    TextureFilterMode::Code minFilter = TextureFilterMode::Nearest;
    TextureWrapMode::Code wrapU = TextureWrapMode::Repeat;
    TextureWrapMode::Code wrapV = TextureWrapMode::Repeat;
    TextureWrapMode::Code wrapW = TextureWrapMode::Repeat;
    StaticArray<intptr_t, GfxConfig::MaxInflightFrames> nativeTextures;
    ImageContent content;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PassDesc
    @ingroup Gfx
    @brief creation attributes for render pass resource
*/
struct PassDesc {
    PassDesc() { };
    PassDesc(const PassDesc& rhs) {
        *this = rhs;
    }
    PassDesc& Locator(const class Locator& loc) {
        locator = loc; return *this;
    }
    const class Locator& Locator() const {
        return locator;
    }
    PassDesc& ColorAttachment(int slotIndex, const Id& tex, int mipLevel=0, int faceLayerSlice=0) {
        auto& att = colorAttachments[slotIndex];
        att.Texture = tex;
        att.MipLevel = mipLevel;
        att.Face = faceLayerSlice;
        return *this;
    }
    PassDesc& DepthStencilAttachment(const Id& tex, int mipLevel=0, int faceLayerSlice=0) {
        auto& att = depthStencilAttachment;
        att.Texture = tex;
        att.MipLevel = mipLevel;
        att.Face = faceLayerSlice;
        return *this;
    }

    class Locator locator = Locator::NonShared();
    struct Attachment {
        Id Texture;
        int MipLevel = 0;
        union {
            int Face = 0;
            int Layer;
            int Slice;
        };
    };
    StaticArray<Attachment, GfxConfig::MaxNumColorAttachments> colorAttachments;
    Attachment depthStencilAttachment;
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

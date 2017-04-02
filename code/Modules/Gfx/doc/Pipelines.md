## Pipeline Objects

Pipeline objects (aka Pipeline State Objects or PSOs) encapsulate
all render state except into a single, immutable object so 
that the entire render pipeline configuration can be 
changed with a single API call.

The following 'state' goes into a pipeline object:

* **a shader** containing a vertex- and fragment-shader 
* up to 4 **vertex layout objects** (one for each vertex buffer resource binding slot) 
* the **primitive type** to use for rendering (triangle list or strips, lines, or points) 
* **depth-stencil-, blend- and rasterizer-state**: collections of granular render-states used to configure the different sections of the render-pipeline

### Related Types

#### VertexLayout

The VertexLayout class is described in detail in the [Meshes documentation](Meshes.md).

#### PrimitiveType

The PrimitiveType enum class defines how the input vertex data
is used to assemble primitives:

* **PrimitiveType::Points**: separate points
* **PrimitiveType::Lines**: separate lines
* **PrimitiveType::LineStrip**: line strips
* **PrimitiveType::Triangles**: separate triangles
* **PrimitiveType::TriangleStrip**: a triangle strip

These are the common primitive types supported across all
underlying 3D-APIs.

#### DepthStencilState

The _DepthStencilState_ structure encapsulates all state required
for depth/stencil-buffer operations:

```cpp
struct DepthStencilState {
    StencilState StencilFront;
    StencilState StencilBack;
    CompareFunc::Code DepthCmpFunc = CompareFunc::Always;
    bool DepthWriteEnabled = false;
    bool StencilEnabled = false;
    uint8_t StencilReadMask = 0xFF;
    uint8_t StencilWriteMask = 0xFF;
    uint8_t StencilRef = 0;
};
```

#### StencilState

_StencilState_ is the render state for stencil-buffer operations that
can be configured separately for front- and back-facing polygons. Two
_StencilState_ structs are embedded in the _DepthStencilState_ structure:

```cpp
struct StencilState {
    StencilOp::Code FailOp = StencilOp::Keep;
    StencilOp::Code DepthFailOp = StencilOp::Keep;
    StencilOp::Code PassOp = StencilOp::Keep;
    CompareFunc::Code CmpFunc = CompareFunc::Always;
};
```

#### CompareFunc

This is an enum with comparison functions for depth- and
stencil-tests.

```cpp
struct CompareFunc {
    enum Code {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };
};
```

#### StencilOp

This is an enum with operations on the current stencil buffer
value:

```cpp
struct StencilOp {
    enum Code {
        Keep,
        Zero,
        Replace,
        IncrClamp,
        DecrClamp,
        Invert,
        IncrWrap,
        DecrWrap,
    };
};
```

#### BlendState

The BlendState struct contains all state to configure the
alpha-blending stage. Separate blending operations
can be configured for the RGB and Alpha pixel channels:

```cpp
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
    uint8_t MRTCount = 1;
};
```

The _ColorFormat_ and _DepthFormat_ members must be configured to
the render target format this pipeline object is compatible with.

The _MRTCount_ must be the number of render targets if
Multiple-Render-Target rendering is used.

Note that this means that all render targets in a multiple-render-target
configuration must have the same color format!

#### BlendFactor

The _BlendFactor_ enum describes the source- or destination-factor
to use in the blending equation:

```cpp
struct BlendFactor {
    enum Code {
        Zero,
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
};
```

#### BlendOperation

The _BlendOperation_ enum defines the operation in the 
blending equation:

```cpp
struct BlendOperation {
    enum Code {
        Add,
        Subtract,
        ReverseSubtract,
    };
};
```

#### PixelChannel

This is a simple enum for the red, green, blue, alpha and
depth, stencil channels and their combinations.

```cpp
struct PixelChannel {
    typedef uint8_t Mask;
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
```

#### RasterizerState

The _RasterizerState_ struct holds all state to configure the
rasterizer stage:

```cpp
struct RasterizerState {
    bool CullFaceEnabled = false;
    bool ScissorTestEnabled = false;
    bool DitherEnabled = true;
    bool AlphaToCoverageEnabled = false;
    Face::Code CullFace = Face::Back;
    uint8_t SampleCount = 1;
};
```

The member _SampleCount_ must match the MSAA sample count of the
render target configuration the rasterizer state is used with.

### Creating Pipeline Objects

TODO!
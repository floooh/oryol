//------------------------------------------------------------------------------
//  Gfx/Core/Types.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "GfxTypes.h"
#include <cstring>

namespace Oryol {

//------------------------------------------------------------------------------
int IndexType::ByteSize(IndexType::Code c) {
    switch (c) {
        case None:      return 0;
        case UInt16:    return 2;
        case UInt32:    return 4;
        default:
            o_error("IndexType::ByteSize() called with invalid type!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
bool PixelFormat::IsValidRenderTargetColorFormat(Code c) {
    switch (c) {
        case RGBA8:
        case R10G10B10A2:
        case RGBA32F:
        case RGBA16F:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
bool PixelFormat::IsValidRenderTargetDepthFormat(Code c) {
    switch (c) {
        case DEPTH:
        case DEPTHSTENCIL:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
bool PixelFormat::IsValidTextureColorFormat(Code c) {
    switch (c) {
        case DEPTH:
        case DEPTHSTENCIL:
            return false;
        default:
            return true;
    }
}

//------------------------------------------------------------------------------
bool PixelFormat::IsValidTextureDepthFormat(Code c) {
    switch (c) {
        case DEPTH:
        case DEPTHSTENCIL:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
bool PixelFormat::IsDepthFormat(Code c) {
    return DEPTH == c;
}

//------------------------------------------------------------------------------
bool PixelFormat::IsDepthStencilFormat(Code c) {
    return DEPTHSTENCIL == c;
}

//------------------------------------------------------------------------------
bool PixelFormat::IsCompressedFormat(Code c) {
    switch (c) {
        case DXT1:
        case DXT3:
        case DXT5:
        case PVRTC2_RGB:
        case PVRTC4_RGB:
        case PVRTC2_RGBA:
        case PVRTC4_RGBA:
        case ETC2_RGB8:
        case ETC2_SRGB8:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
bool PixelFormat::IsPVRTC(Code c) {
    switch (c) {
        case PVRTC2_RGB:
        case PVRTC4_RGB:
        case PVRTC2_RGBA:
        case PVRTC4_RGBA:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
bool PixelFormat::IsDXT(Code c) {
    switch (c) {
        case DXT1:
        case DXT3:
        case DXT5:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
bool PixelFormat::IsETC2(Code c) {
    switch (c) {
        case ETC2_RGB8:
        case ETC2_SRGB8:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
int PixelFormat::ByteSize(Code c) {
    switch (c) {
        case RGBA32F:
            return 16;
        case RGBA16F:
            return 8;
        case RGBA8:
        case R10G10B10A2:
        case R32F:
            return 4;
        case RGB8:
            return 3;
        case R5G6B5:
        case R5G5B5A1:
        case RGBA4:
        case R16F:
            return 2;
        case L8:
            return 1;
        case DEPTH:
            // NOTE: this is not true on all platform!
            return 2;
        case DEPTHSTENCIL:
            return 4;
        default:
            o_error("PixelFormat::ByteSize(): cannot get byte size for compressed format!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
int8_t PixelFormat::NumBits(Code pixelFormat, PixelChannel::Bits channel) {
    switch (pixelFormat) {
        case RGBA32F:
            if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel) || (PixelChannel::Alpha == channel)) {
                return 32;
            }
            break;
        case RGBA16F:
            if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel) || (PixelChannel::Alpha == channel)) {
                return 16;
            }
            break;
        case R32F:
            if (PixelChannel::Red == channel) {
                return 32;
            }
            break;
        case R16F:
            if (PixelChannel::Red == channel) {
                return 16;
            }
            break;
        case R10G10B10A2:
            if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel)) {
                return 10;
            }
            else if (PixelChannel::Alpha == channel) {
                return 2;
            }
            break;
        case RGBA8:
            if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel) || (PixelChannel::Alpha == channel)) {
                return 8;
            }
            break;
        case RGB8:
            if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel)) {
                return 8;
            }
            break;
        case R5G6B5:
            if ((PixelChannel::Red == channel) || (PixelChannel::Blue == channel)) {
                return 5;
            }
            else if (PixelChannel::Green == channel) {
                return 6;
            }
            break;
        case R5G5B5A1:
            if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel)) {
                return 5;
            }
            else if (PixelChannel::Alpha == channel) {
                return 1;
            }
            break;
        case RGBA4:
            if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel) || (PixelChannel::Alpha == channel)) {
                return 4;
            }
            break;
        case L8:
            if (PixelChannel::Red == channel) {
                return 8;
            }
            break;
        case DEPTH:
            // NOTE: this is not true on all platforms!
            if (PixelChannel::Depth == channel) {
                return 16;
            }
            break;
        case DEPTHSTENCIL:
            if (PixelChannel::Depth == channel) {
                return 24;
            }
            else if (PixelChannel::Stencil == channel) {
                return 8;
            }
            break;
        default:
            break;
    }
    // fallthrough: unsupported combination
    return 0;
}

//------------------------------------------------------------------------------
int PixelFormat::RowPitch(PixelFormat::Code fmt, int width) {
    int pitch;
    switch (fmt) {
        case PixelFormat::DXT1:
        case PixelFormat::ETC2_RGB8:
        case PixelFormat::ETC2_SRGB8:
            pitch = ((width + 3) / 4) * 8;
            pitch = pitch < 8 ? 8 : pitch;
            break;
        case PixelFormat::DXT3:
        case PixelFormat::DXT5:
            pitch = ((width + 3) / 4) * 16;
            pitch = pitch < 16 ? 16 : pitch;
            break;
        case PixelFormat::PVRTC4_RGB:
        case PixelFormat::PVRTC4_RGBA:
            {
                const int blockSize = 4*4;
                const int bpp = 4;
                int widthBlocks = width / 4;
                widthBlocks = widthBlocks < 2 ? 2 : widthBlocks;
                pitch = widthBlocks * ((blockSize * bpp) / 8);
            }
            break;
        case PixelFormat::PVRTC2_RGB:
        case PixelFormat::PVRTC2_RGBA:
            {
                const int blockSize = 8 * 4;
                const int bpp = 2;
                int widthBlocks = width / 4;
                widthBlocks = widthBlocks < 2 ? 2 : widthBlocks;
                pitch = widthBlocks * ((blockSize * bpp) / 8);
            }
            break;
        default:
            pitch = width * PixelFormat::ByteSize(fmt);
            break;
    }
    return pitch;
}

//------------------------------------------------------------------------------
int PixelFormat::ImagePitch(PixelFormat::Code fmt, int width, int height) {
    int numRows = 0;
    switch (fmt) {
        case PixelFormat::DXT1:
        case PixelFormat::ETC2_RGB8:
        case PixelFormat::ETC2_SRGB8:
        case PixelFormat::DXT3:
        case PixelFormat::DXT5:
        case PixelFormat::PVRTC4_RGB:
        case PixelFormat::PVRTC4_RGBA:
        case PixelFormat::PVRTC2_RGB:
        case PixelFormat::PVRTC2_RGBA:
            numRows = ((height + 3) / 4);
            break;
        default:
            numRows = height;
            break;
    }
    if (numRows < 1) {
        numRows = 1;
    }
    const int pitch = numRows * RowPitch(fmt, width);
    return pitch;
}

//------------------------------------------------------------------------------
const char* PrimitiveType::ToString(PrimitiveType::Code c) {
    switch (c) {
        case Points:        return "Points";
        case Lines:         return "Lines";
        case LineStrip:     return "LineStrip";
        case Triangles:     return "Triangles";
        case TriangleStrip: return "TriangleStrip";
        default:
            o_error("PrimitiveType::ToString(): invalid value!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
int VertexFormat::ByteSize(Code c) {
    switch (c) {
        case Float:
            return 4;
        case Float2:
            return 8;
        case Float3:
            return 12;
        case Float4:
            return 16;
        case Byte4:
        case Byte4N:
        case UByte4:
        case UByte4N:
        case Short2:
        case Short2N:
        case UInt10_2N:
            return 4;
        case Short4:
        case Short4N:
            return 8;
        default:
            o_error("VertexFormat::ByteSize() called with invalid format!\n");
            return 0;
    }
}

//------------------------------------------------------------------------------
const char* VertexFormat::ToString(Code c) {
    switch (c) {
        case Float:     return "Float";
        case Float2:    return "Float2";
        case Float3:    return "Float3";
        case Float4:    return "Float4";
        case Byte4:     return "Byte4";
        case Byte4N:    return "Byte4N";
        case UByte4:    return "UByte4";
        case UByte4N:   return "Ubyte4N";
        case Short2:    return "Short2";
        case Short2N:   return "Short2N";
        case Short4:    return "Short4";
        case Short4N:   return "Short4N";
        case UInt10_2N: return "Int10_2N";
        default:
            o_error("VertexFormat::ToString(): invalid value!\n");
            return nullptr;
    }
}

//------------------------------------------------------------------------------
PassAction::PassAction() {
    for (auto& c : this->Color) {
        c = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

//------------------------------------------------------------------------------
PassAction PassAction::Clear(const glm::vec4& c, float d, uint8_t s) {
    PassAction p;
    p.Color.Fill(c);
    p.Depth = d;
    p.Stencil = s;
    return p;
}

//------------------------------------------------------------------------------
PassAction PassAction::Clear(std::initializer_list<glm::vec4> colors, float d, uint8_t s) {
    PassAction p;
    int i = 0;
    for (const auto& c : colors) {
        p.Color[i++] = c;
    }
    p.Depth = d;
    p.Stencil = s;
    return p;
}

//------------------------------------------------------------------------------
PassAction PassAction::Load() {
    PassAction p;
    p.Flags = LoadC0|LoadC1|LoadC2|LoadC3|LoadDS;
    return p;
}

//------------------------------------------------------------------------------
PassAction PassAction::DontCare() {
    PassAction p;
    p.Flags = 0;
    return p;
}

//------------------------------------------------------------------------------
PassAction& PassAction::ClearColor(int index, const glm::vec4& color) {
    this->Color[index] = color;
    this->Flags &= ~((ClearC0|LoadC0)<<index);
    this->Flags |= ClearC0<<index; 
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::ClearDepthStencil(float d, uint8_t s) {
    this->Depth = d;
    this->Stencil = s;
    this->Flags &= ~(ClearDS|LoadDS);
    this->Flags |= ClearDS;
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::DontCareColor(int index) {
    o_assert_range_dbg(index, GfxConfig::MaxNumColorAttachments);
    this->Flags &= ~((ClearC0|LoadC0)<<index);
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::DontCareDepthStencil() {
    this->Flags &= ~(ClearDS|LoadDS);
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::LoadColor(int index) {
    o_assert_range_dbg(index, GfxConfig::MaxNumColorAttachments);
    this->Flags &= ~((ClearC0|LoadC0)<<index);
    this->Flags |= LoadC0<<index;
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::LoadDepthStencil() {
    this->Flags &= ~(ClearDS|LoadDS);
    this->Flags |= LoadDS;
    return *this;
}

//------------------------------------------------------------------------------
VertexLayout::VertexLayout() {
    this->Clear();
}

//------------------------------------------------------------------------------
VertexLayout::VertexLayout(std::initializer_list<Component> l) {
    this->Clear();
    for (const auto& c : l) {
        this->Add(c);
    }
}

//------------------------------------------------------------------------------
VertexLayout& VertexLayout::Clear() {
    this->StepFunction = VertexStepFunction::PerVertex;
    this->StepRate = 1;
    this->numComps = 0;
    this->byteSize = 0;
    return *this;
}

//------------------------------------------------------------------------------
bool VertexLayout::Empty() const {
    return 0 == this->numComps;
}

//------------------------------------------------------------------------------
VertexLayout& VertexLayout::Add(const Component& comp) {
    o_assert_dbg(this->numComps < GfxConfig::MaxNumVertexLayoutComponents);
    this->comps[this->numComps] = comp;
    this->comps[this->numComps].Offset = this->byteSize;
    this->byteSize += comp.ByteSize();
    this->numComps++;
    return *this;
}

//------------------------------------------------------------------------------
VertexLayout& VertexLayout::Add(VertexFormat::Code format) {
    return this->Add(Component(format));
}

//------------------------------------------------------------------------------
VertexLayout& VertexLayout::Add(const StringAtom& name, VertexFormat::Code format) {
    return this->Add(Component(name, format));
}

//------------------------------------------------------------------------------
VertexLayout& VertexLayout::Add(std::initializer_list<Component> l) {
    for (const auto& c : l) {
        this->Add(c);
    }
    return *this;
}

//------------------------------------------------------------------------------
VertexLayout& VertexLayout::EnableInstancing() {
    this->StepFunction = VertexStepFunction::PerInstance;
    this->StepRate = 1;
    return *this;
}

//------------------------------------------------------------------------------
int VertexLayout::NumComponents() const {
    return this->numComps;
}

//------------------------------------------------------------------------------
const VertexLayout::Component& VertexLayout::ComponentAt(int index) const {
    return this->comps[index];
}

//------------------------------------------------------------------------------
int VertexLayout::ComponentIndexByName(const StringAtom& name) const {
    for (int i = 0; i < this->numComps; i++) {
        if (this->comps[i].Name == name) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
bool VertexLayout::Contains(const StringAtom& name) const {
    return InvalidIndex != this->ComponentIndexByName(name);
}

//------------------------------------------------------------------------------
int VertexLayout::ByteSize() const {
    return this->byteSize;
}

//------------------------------------------------------------------------------
int VertexLayout::ComponentByteOffset(int componentIndex) const {
    o_assert_dbg(componentIndex < this->numComps);
    return this->comps[componentIndex].Offset;
}

//------------------------------------------------------------------------------
DisplayAttrs DisplayAttrs::FromTextureAttrs(const TextureAttrs& texAttrs) {
    DisplayAttrs dispAttrs;
    dispAttrs.WindowWidth = texAttrs.Width;
    dispAttrs.WindowHeight = texAttrs.Height;
    dispAttrs.WindowPosX = 0;
    dispAttrs.WindowPosY = 0;
    dispAttrs.FramebufferWidth = texAttrs.Width;
    dispAttrs.FramebufferHeight = texAttrs.Height;
    dispAttrs.ColorPixelFormat = texAttrs.ColorFormat;
    dispAttrs.DepthPixelFormat = texAttrs.DepthFormat;
    dispAttrs.SampleCount = texAttrs.SampleCount;
    dispAttrs.Windowed = false;
    dispAttrs.SwapInterval = 1;
    return dispAttrs;
}

//------------------------------------------------------------------------------
ImageDataAttrs::ImageDataAttrs() {
    for (auto& offsets : this->Offsets) {
        offsets.Fill(0);
    }
    for (auto& sizes : this->Sizes) {
        sizes.Fill(0);
    }
}

//------------------------------------------------------------------------------
int IndexBufferAttrs::ByteSize() const {
    return NumIndices * IndexType::ByteSize(Type);
}

//------------------------------------------------------------------------------
int VertexBufferAttrs::ByteSize() const {
    return NumVertices * Layout.ByteSize();
}

//------------------------------------------------------------------------------
GfxSetup GfxSetup::Window(int width, int height, String windowTitle) {
    o_assert_dbg((width > 0) && (height > 0));
    GfxSetup setup;
    setup.Width    = width;
    setup.Height   = height;
    setup.Windowed = true;
    setup.Title    = windowTitle;
    return setup;
}

//------------------------------------------------------------------------------
GfxSetup GfxSetup::Fullscreen(int width, int height, String windowTitle) {
    o_assert_dbg((width > 0) && (height > 0));
    GfxSetup setup;
    setup.Width    = width;
    setup.Height   = height;
    setup.Windowed = false;
    setup.Title    = windowTitle;
    return setup;
}

//------------------------------------------------------------------------------
GfxSetup GfxSetup::WindowMSAA4(int width, int height, String windowTitle) {
    GfxSetup setup = Window(width, height, windowTitle);
    setup.SampleCount = 4;
    return setup;
}

//------------------------------------------------------------------------------
GfxSetup GfxSetup::FullscreenMSAA4(int width, int height, String windowTitle) {
    GfxSetup setup = Fullscreen(width, height, windowTitle);
    setup.SampleCount = 4;
    return setup;
}

//------------------------------------------------------------------------------
DisplayAttrs GfxSetup::GetDisplayAttrs() const {
    DisplayAttrs attrs;
    attrs.WindowWidth       = this->Width;
    attrs.WindowHeight      = this->Height;
    attrs.WindowPosX        = 0;
    attrs.WindowPosY        = 0;
    attrs.FramebufferWidth  = this->Width;
    attrs.FramebufferHeight = this->Height;
    attrs.ColorPixelFormat  = this->ColorFormat;
    attrs.DepthPixelFormat  = this->DepthFormat;
    attrs.SampleCount       = this->SampleCount;
    attrs.Windowed          = this->Windowed;
    attrs.WindowTitle       = this->Title;
    attrs.SwapInterval      = this->SwapInterval;
    return attrs;
}

//------------------------------------------------------------------------------
GfxSetup::GfxSetup() {
    for (int i = 0; i < GfxResourceType::Num; i++) {
        ResourcePoolSize[i] = GfxConfig::DefaultResourcePoolSize;
        ResourceThrottling[i] = 0;    // unthrottled
    }
}

//------------------------------------------------------------------------------
BufferSetup::BufferSetup() {
    this->NativeBuffers.Fill(0);
}

//------------------------------------------------------------------------------
BufferSetup
BufferSetup::Make(int size, BufferType::Code type, Usage::Code usage) {
    o_assert_dbg(size > 0);
    BufferSetup setup;
    setup.Size = size;
    setup.Type = type;
    setup.Usage = usage;
    return setup;
}

//------------------------------------------------------------------------------
PipelineSetup PipelineSetup::FromShader(const Id& shd) {
    o_assert_dbg(shd.IsValid());
    PipelineSetup setup;
    setup.Shader = shd;
    return setup;
}

//------------------------------------------------------------------------------
PipelineSetup PipelineSetup::FromShaderAndLayout(const Id& shd, const VertexLayout& layout) {
    o_assert_dbg(!layout.Empty() && shd.IsValid());
    PipelineSetup setup;
    setup.Layouts[0] = layout;
    setup.Shader = shd;
    return setup;
}

//------------------------------------------------------------------------------
PassSetup PassSetup::From(Id colorTexture, Id depthStencilTexture) {
    PassSetup setup;
    setup.ColorAttachments[0].Texture = colorTexture;
    setup.DepthStencilTexture = depthStencilTexture;
    return setup;
}

//------------------------------------------------------------------------------
PassSetup PassSetup::From(std::initializer_list<Id> colorTextures, Id depthStencilTexture) {
    PassSetup setup;
    int i = 0;
    for (const auto& id : colorTextures) {
        setup.ColorAttachments[i++].Texture = id;
    }
    setup.DepthStencilTexture = depthStencilTexture;
    return setup;
}

//------------------------------------------------------------------------------
void ShaderSetup::SetProgramFromSources(ShaderLang::Code slang, const String& vsSource, const String& fsSource) {
    o_assert_dbg(vsSource.IsValid() && fsSource.IsValid());
    this->program.vsSources[slang] = vsSource;
    this->program.fsSources[slang] = fsSource;
}

//------------------------------------------------------------------------------
void ShaderSetup::SetProgramFromByteCode(ShaderLang::Code slang, const uint8_t* vsByteCode, uint32_t vsNumBytes, const uint8_t* fsByteCode, uint32_t fsNumBytes, const char* vsFunc, const char* fsFunc) {
    o_assert_dbg(vsByteCode && (vsNumBytes > 0));
    o_assert_dbg(fsByteCode && (fsNumBytes > 0));
    this->program.vsByteCode[slang].ptr = vsByteCode;
    this->program.vsByteCode[slang].size = vsNumBytes;
    this->program.fsByteCode[slang].ptr = fsByteCode;
    this->program.fsByteCode[slang].size = fsNumBytes;
    if (vsFunc) {
        this->program.vsFuncs[slang] = vsFunc;
    }
    else {
        this->program.vsFuncs[slang].Clear();
    }
    if (fsFunc) {
        this->program.fsFuncs[slang] = fsFunc;
    }
    else {
        this->program.fsFuncs[slang].Clear();
    }
}

//------------------------------------------------------------------------------
void ShaderSetup::AddUniformBlock(const StringAtom& type, const StringAtom& name, uint32_t typeHash, uint32_t byteSize, ShaderStage::Code bindStage, int32_t bindSlot) {
    o_assert_dbg(type.IsValid());
    o_assert_dbg(bindSlot >= 0);
    uniformBlockEntry& entry = this->uniformBlocks[this->numUniformBlocks++];
    entry.type = type;
    entry.name = name;
    entry.typeHash = typeHash;
    entry.byteSize = byteSize;
    entry.bindStage = bindStage;
    entry.bindSlot = bindSlot;
}

//------------------------------------------------------------------------------
void ShaderSetup::AddTexture(const StringAtom& name, TextureType::Code type, ShaderStage::Code bindStage, int32_t bindSlot) {
    o_assert_dbg(name.IsValid());
    o_assert_dbg(bindSlot >= 0);
    textureEntry& entry = this->textures[this->numTextures++];
    entry.name = name;
    entry.type = type;
    entry.bindStage = bindStage;
    entry.bindSlot = bindSlot;
}

//------------------------------------------------------------------------------
const String& ShaderSetup::VertexShaderSource(ShaderLang::Code slang) const {
    return this->program.vsSources[slang];
}

//------------------------------------------------------------------------------
const String& ShaderSetup::FragmentShaderSource(ShaderLang::Code slang) const {
    return this->program.fsSources[slang];
}

//------------------------------------------------------------------------------
void ShaderSetup::VertexShaderByteCode(ShaderLang::Code slang, const void*& outPtr, uint32_t& outSize) const {
    outPtr = this->program.vsByteCode[slang].ptr;
    outSize = this->program.vsByteCode[slang].size;
}

//------------------------------------------------------------------------------
void ShaderSetup::FragmentShaderByteCode(ShaderLang::Code slang, const void*& outPtr, uint32_t& outSize) const {
    outPtr = this->program.fsByteCode[slang].ptr;
    outSize = this->program.fsByteCode[slang].size;
}

//------------------------------------------------------------------------------
const StringAtom& ShaderSetup::VertexShaderFunc(ShaderLang::Code slang) const {
    return this->program.vsFuncs[slang];
}

//------------------------------------------------------------------------------
const StringAtom& ShaderSetup::FragmentShaderFunc(ShaderLang::Code slang) const {
    return this->program.fsFuncs[slang];
}

//------------------------------------------------------------------------------
int ShaderSetup::NumUniformBlocks() const {
    return this->numUniformBlocks;
}

//------------------------------------------------------------------------------
int ShaderSetup::UniformBlockIndexByStageAndSlot(ShaderStage::Code bindStage, int bindSlot) const {
    for (int i = 0; i < this->numUniformBlocks; i++) {
        const auto& entry = this->uniformBlocks[i];
        if ((entry.bindStage == bindStage) && (entry.bindSlot == bindSlot)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
const StringAtom& ShaderSetup::UniformBlockName(int index) const {
    return this->uniformBlocks[index].name;
}

//------------------------------------------------------------------------------
const StringAtom& ShaderSetup::UniformBlockType(int index) const {
    return this->uniformBlocks[index].type;
}

//------------------------------------------------------------------------------
uint32_t ShaderSetup::UniformBlockTypeHash(int index) const {
    return this->uniformBlocks[index].typeHash;
}

//------------------------------------------------------------------------------
uint32_t ShaderSetup::UniformBlockByteSize(int index) const {
    return this->uniformBlocks[index].byteSize;
}

//------------------------------------------------------------------------------
ShaderStage::Code ShaderSetup::UniformBlockBindStage(int index) const {
    return this->uniformBlocks[index].bindStage;
}

//------------------------------------------------------------------------------
int ShaderSetup::UniformBlockBindSlot(int index) const {
    return this->uniformBlocks[index].bindSlot;
}

//------------------------------------------------------------------------------
int ShaderSetup::NumTextures() const {
    return this->numTextures;
}

//------------------------------------------------------------------------------
int ShaderSetup::TextureIndexByStageAndSlot(ShaderStage::Code bindStage, int bindSlot) const {
    for (int i = 0; i < this->numTextures; i++) {
        const auto& entry = this->textures[i];
        if ((entry.bindStage == bindStage) && (entry.bindSlot == bindSlot)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
const StringAtom& ShaderSetup::TexName(int index) const {
    return this->textures[index].name;
}

//------------------------------------------------------------------------------
TextureType::Code ShaderSetup::TexType(int index) const {
    return this->textures[index].type;
}

//------------------------------------------------------------------------------
ShaderStage::Code ShaderSetup::TexBindStage(int index) const {
    return this->textures[index].bindStage;
}

//------------------------------------------------------------------------------
int ShaderSetup::TexBindSlot(int index) const {
    return this->textures[index].bindSlot;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::FromPixelData2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    TextureSetup setup(blueprint);
    setup.setupFromPixelData = true;
    setup.Type = TextureType::Texture2D;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.ImageData.NumFaces = 1;
    setup.ImageData.NumMipMaps = numMipMaps;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::FromPixelDataCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    TextureSetup setup(blueprint);
    setup.setupFromPixelData = true;
    setup.Type = TextureType::TextureCube;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.ImageData.NumFaces = 6;
    setup.ImageData.NumMipMaps = numMipMaps;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::FromPixelData3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0) && (d > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    TextureSetup setup(blueprint);
    setup.setupFromPixelData = true;
    setup.Type = TextureType::Texture3D;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = d;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.ImageData.NumFaces = 1;
    setup.ImageData.NumMipMaps = numMipMaps;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::FromPixelDataArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0) && (layers > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    TextureSetup setup(blueprint);
    setup.setupFromPixelData = true;
    setup.Type = TextureType::TextureArray;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = layers;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.ImageData.NumFaces = 1;
    setup.ImageData.NumMipMaps = numMipMaps;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::Empty2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    TextureSetup setup(blueprint);
    setup.setupEmpty = true;
    setup.Type = TextureType::Texture2D;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::EmptyCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    TextureSetup setup(blueprint);
    setup.setupEmpty = true;
    setup.Type = TextureType::TextureCube;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::Empty3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0) && (d > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    TextureSetup setup(blueprint);
    setup.setupEmpty = true;
    setup.Type = TextureType::Texture3D;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = d;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::EmptyArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint) {
    o_assert_dbg((w > 0) && (h > 0) && (layers > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    TextureSetup setup(blueprint);
    setup.setupEmpty = true;
    setup.Type = TextureType::TextureArray;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = layers;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::RenderTarget2D(int w, int h, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert_dbg((w > 0) && (h > 0));
    TextureSetup setup;
    setup.Type = TextureType::Texture2D;
    setup.IsRenderTarget = true;
    setup.Width = w;
    setup.Height = h;
    setup.ColorFormat = colorFmt;
    setup.DepthFormat = depthFmt;
    setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::RenderTargetCube(int w, int h, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert_dbg((w > 0) && (h > 0));
    TextureSetup setup;
    setup.Type = TextureType::TextureCube;
    setup.IsRenderTarget = true;
    setup.Width = w;
    setup.Height = h;
    setup.ColorFormat = colorFmt;
    setup.DepthFormat = depthFmt;
    setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::RenderTarget3D(int w, int h, int d, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert_dbg((w > 0) && (h > 0));
    TextureSetup setup;
    setup.Type = TextureType::Texture3D;
    setup.IsRenderTarget = true;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = d;
    setup.ColorFormat = colorFmt;
    setup.DepthFormat = depthFmt;
    setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::RenderTargetArray(int w, int h, int layers, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt) {
    o_assert_dbg((w > 0) && (h > 0));
    TextureSetup setup;
    setup.Type = TextureType::TextureArray;
    setup.IsRenderTarget = true;
    setup.Width = w;
    setup.Height = h;
    setup.Depth = layers;
    setup.ColorFormat = colorFmt;
    setup.DepthFormat = depthFmt;
    setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    return setup;
}

//------------------------------------------------------------------------------
TextureSetup TextureSetup::FromNativeTexture(int w, int h, int numMipMaps, TextureType::Code type, PixelFormat::Code fmt, Usage::Code usage, intptr_t h0, intptr_t h1) {
    o_assert_dbg((w > 0) && (h > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
    o_assert((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
    o_assert_dbg(h0 != 0);
    TextureSetup setup;
    setup.setupFromNativeHandle = true;
    setup.Type = type;
    setup.Width = w;
    setup.Height = h;
    setup.NumMipMaps = numMipMaps;
    setup.ColorFormat = fmt;
    setup.TextureUsage = usage;
    setup.NativeHandle[0] = h0;
    setup.NativeHandle[1] = h1;
    return setup;
}

//------------------------------------------------------------------------------
bool TextureSetup::ShouldSetupFromPixelData() const {
    return this->setupFromPixelData;
}

//------------------------------------------------------------------------------
bool TextureSetup::ShouldSetupFromNativeTexture() const {
    return this->setupFromNativeHandle;
}

//------------------------------------------------------------------------------
bool TextureSetup::ShouldSetupEmpty() const {
    return this->setupEmpty;
}

//------------------------------------------------------------------------------
bool TextureSetup::HasDepth() const {
    return this->DepthFormat != PixelFormat::Invalid;
}

//------------------------------------------------------------------------------
TextureSetup::TextureSetup() {
    NativeHandle.Fill(0);
}

} // namespace Oryol

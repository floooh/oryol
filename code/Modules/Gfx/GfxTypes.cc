//------------------------------------------------------------------------------
//  Gfx/Core/Types.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "GfxTypes.h"
#include "Gfx.h"
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
PrimitiveGroup::PrimitiveGroup(int baseElm, int numElms):
    BaseElement(baseElm),
    NumElements(numElms)
{ }

//------------------------------------------------------------------------------
PrimitiveGroup& PrimitiveGroup::SetBaseElement(int baseElm) {
    this->BaseElement = baseElm;
    return *this;
}

//------------------------------------------------------------------------------
PrimitiveGroup& PrimitiveGroup::SetNumElements(int numElms) {
    this->NumElements = numElms;
    return *this;
}

//------------------------------------------------------------------------------
PassAction::PassAction() {
    for (auto& c : this->Color) {
        c = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

//------------------------------------------------------------------------------
PassAction& PassAction::Load() {
    this->Flags = LoadC0|LoadC1|LoadC2|LoadC3|LoadDS;
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::DontCare() {
    this->Flags = 0;
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::ClearColor(int index, float r, float g, float b, float a) {
    o_assert_range_dbg(index, GfxConfig::MaxNumColorAttachments);
    this->Color[index] = glm::vec4(r, g, b, a);
    this->Flags |= (ClearC0<<index);
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::ClearColor(int index, const glm::vec4& color) {
    o_assert_range_dbg(index, GfxConfig::MaxNumColorAttachments);
    this->Color[index] = color;
    this->Flags |= (ClearC0<<index);
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::ClearColor(float r, float g, float b, float a) {
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        this->ClearColor(i, r, g, b, a);
    }
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::ClearColor(const glm::vec4& color) {
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        this->ClearColor(i, color);
    }
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::ClearDepthStencil(float d, uint8_t s) {
    this->Depth = d;
    this->Stencil = s;
    this->Flags |= ClearDS;
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::Clear(float r, float g, float b, float a, float d, uint8_t s) {
    this->ClearColor(r, g, b, a);
    this->ClearDepthStencil(d, s);
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::Clear(const glm::vec4& c, float d, uint8_t s) {
    this->ClearColor(c);
    this->ClearDepthStencil(d, s);
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::Clear(std::initializer_list<glm::vec4> colors, float d, uint8_t s) {
    int i = 0;
    for (const auto& c : colors) {
        this->ClearColor(i++, c);
    }
    this->ClearDepthStencil(d, s);
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::LoadColor(int index) {
    o_assert_range_dbg(index, GfxConfig::MaxNumColorAttachments);
    this->Flags |= LoadC0<<index;
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::LoadColor() {
    this->Flags |= LoadC0|LoadC1|LoadC2|LoadC3;
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::LoadDepthStencil() {
    this->Flags |= LoadDS;
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::DontCareColor(int index) {
    o_assert_range_dbg(index, GfxConfig::MaxNumColorAttachments);
    this->Flags &= ~(ClearC0|LoadC0<<index);
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::DontCareColor() {
    this->Flags &= ~(ClearC0|ClearC1|ClearC2|ClearC3|LoadC0|LoadC1|LoadC2|LoadC3);
    return *this;
}

//------------------------------------------------------------------------------
PassAction& PassAction::DontCareDepthStencil() {
    this->Flags &= ~(ClearDS|LoadDS);
    return *this;
}

//------------------------------------------------------------------------------
Bindings::Bindings() {
    this->VertexBufferOffsets.Fill(0);
}

//------------------------------------------------------------------------------
Bindings& Bindings::SetVertexBuffer(int slot, const Id& bufId) {
    o_assert_dbg(!bufId.IsValid() || (bufId.Type == GfxResourceType::Buffer));
    this->VertexBuffers[slot] = bufId;
    return *this;
}

//------------------------------------------------------------------------------
Bindings& Bindings::SetVertexBufferOffset(int slot, int offset) {
    this->VertexBufferOffsets[slot] = offset;
    return *this;
}

//------------------------------------------------------------------------------
Bindings& Bindings::SetIndexBuffer(const Id& bufId) {
    o_assert_dbg(!bufId.IsValid() || (bufId.Type == GfxResourceType::Buffer));
    this->IndexBuffer = bufId;
    return *this;
}

//------------------------------------------------------------------------------
Bindings& Bindings::SetIndexBufferOffset(int offset) {
    this->IndexBufferOffset = offset;
    return *this;
}

//------------------------------------------------------------------------------
Bindings& Bindings::SetVSTexture(int slot, const Id& texId) {
    o_assert_dbg(!texId.IsValid() || (texId.Type == GfxResourceType::Texture));
    this->VSTexture[slot] = texId;
    return *this;
}

//------------------------------------------------------------------------------
Bindings& Bindings::SetFSTexture(int slot, const Id& texId) {
    o_assert_dbg(!texId.IsValid() || (texId.Type == GfxResourceType::Texture));
    this->FSTexture[slot] = texId;
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
ImageContent::ImageContent() {
    for (auto& ptr : this->Pointer) {
        ptr.Fill(0);
    }
    for (auto& size : this->Size) {
        size.Fill(0);
    }
}

//------------------------------------------------------------------------------
ImageContent& ImageContent::SetPointer(int faceIndex, int mipIndex, const void* ptr) {
    this->Pointer[faceIndex][mipIndex] = ptr;
    return *this;
}

//------------------------------------------------------------------------------
ImageContent& ImageContent::SetSize(int faceIndex, int mipIndex, int size) {
    this->Size[faceIndex][mipIndex] = size;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc::GfxDesc() {
    for (int i = 0; i < GfxResourceType::Num; i++) {
        this->ResourcePoolSize[i] = GfxConfig::DefaultResourcePoolSize;
    }
}

//------------------------------------------------------------------------------
GfxDesc::GfxDesc(const GfxDesc& rhs) {
    *this = rhs;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetWidth(int w) {
    this->Width = w;
    return *this; 
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetHeight(int h) {
    this->Height = h;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetColorFormat(PixelFormat::Code fmt) {
    this->ColorFormat = fmt;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetDepthFormat(PixelFormat::Code fmt) {
    this->DepthFormat = fmt;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetSampleCount(int c) {
    this->SampleCount = c;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetWindowed(bool b) {
    this->Windowed = b;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetSwapInterval(int i) {
    this->SwapInterval = i;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetTitle(const StringAtom& t) {
    this->Title = t;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetHighDPI(bool b) {
    this->HighDPI = b;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetHtmlTrackElementSize(bool b) {
    this->HtmlTrackElementSize = b;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetHtmlElement(const StringAtom& e) {
    this->HtmlElement = e;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetResourcePoolSize(GfxResourceType::Code type, int size) {
    this->ResourcePoolSize[type] = size;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetResourceLabelStackCapacity(int c) {
    this->ResourceLabelStackCapacity = c;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetResourceRegistryCapacity(int c) {
    this->ResourceRegistryCapacity = c;
    return *this;
}

//------------------------------------------------------------------------------
GfxDesc& GfxDesc::SetGlobalUniformBufferSize(int s) {
    this->GlobalUniformBufferSize = s;
    return *this;
}

//------------------------------------------------------------------------------
BufferDesc::BufferDesc() {
    this->NativeBuffers.Fill(0);
}

//------------------------------------------------------------------------------
BufferDesc::BufferDesc(const BufferDesc& rhs) {
    *this = rhs;
}

//------------------------------------------------------------------------------
BufferDesc& BufferDesc::SetLocator(const class Locator& l) {
    this->Locator = l;
    return *this;
}

//------------------------------------------------------------------------------
BufferDesc& BufferDesc::SetType(BufferType::Code t) {
    this->Type = t;
    return *this;
}

//------------------------------------------------------------------------------
BufferDesc& BufferDesc::SetUsage(Usage::Code u) {
    this->Usage = u;
    return *this;
}

//------------------------------------------------------------------------------
BufferDesc& BufferDesc::SetSize(int s) {
    this->Size = s; return *this;
}

//------------------------------------------------------------------------------
BufferDesc& BufferDesc::SetContent(const void* c) {
    this->Content = c;
    return *this;
}

//------------------------------------------------------------------------------
BufferDesc& BufferDesc::SetNativeBuffer(int index, intptr_t buf) {
    this->NativeBuffers[index] = buf;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc::PipelineDesc(const PipelineDesc& rhs) {
    *this = rhs;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetLocator(const class Locator& loc) {
    this->Locator = loc;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetShader(const Id& shd) {
    this->Shader = shd;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetLayout(int slotIndex, const VertexLayout& layout) {
    this->Layouts[slotIndex] = layout;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetPrimitiveType(PrimitiveType::Code t) {
    this->PrimType = t;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetIndexType(IndexType::Code t) {
    this->IndexType = t;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetDepthCmpFunc(CompareFunc::Code f) {
    this->DepthCmpFunc = f;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetDepthWriteEnabled(bool b) {
    this->DepthWriteEnabled = b;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetStencilEnabled(bool b) {
    this->StencilEnabled = b;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetStencilReadMask(uint8_t m) {
    this->StencilReadMask = m;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetStencilWriteMask(uint8_t m) {
    this->StencilWriteMask = m;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetStencilRef(uint8_t r) {
    this->StencilRef = r;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetStencilFailOp(Face::Code face, StencilOp::Code op) {
    if (Face::Front & face) {
        this->StencilFrontFailOp = op;
    }
    if (Face::Back & face) {
        this->StencilBackFailOp = op;
    }
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetStencilDepthFailOp(Face::Code face, StencilOp::Code op) {
    if (Face::Front & face) {
        this->StencilFrontDepthFailOp = op;
    }
    if (Face::Back & face) {
        this->StencilBackDepthFailOp = op;
    }
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetStencilPassOp(Face::Code face, StencilOp::Code op) {
    if (Face::Front & face) {
        this->StencilFrontPassOp = op;
    }
    if (Face::Back & face) {
        this->StencilBackPassOp = op;
    }
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetStencilCmpFunc(Face::Code face, CompareFunc::Code fn) {
    if (Face::Front & face) {
        this->StencilFrontCmpFunc = fn;
    }
    if (Face::Back & face) {
        this->StencilBackCmpFunc = fn;
    }
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendEnabled(bool b) {
    this->BlendEnabled = b;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendSrcFactor(BlendFactor::Code f) {
    this->BlendSrcFactorRGB = f;
    this->BlendSrcFactorAlpha = f;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendSrcFactorRGB(BlendFactor::Code f) {
    this->BlendSrcFactorRGB = f;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendSrcFactorAlpha(BlendFactor::Code f) {
    this->BlendSrcFactorAlpha = f;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendDstFactor(BlendFactor::Code f) {
    this->BlendDstFactorRGB = f;
    this->BlendDstFactorAlpha = f;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendDstFactorRGB(BlendFactor::Code f) {
    this->BlendDstFactorRGB = f;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendDstFactorAlpha(BlendFactor::Code f) {
    this->BlendDstFactorAlpha = f;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendOp(BlendOperation::Code op) {
    this->BlendOpRGB = op;
    this->BlendOpAlpha = op;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendOpRGB(BlendOperation::Code op) {
    this->BlendOpRGB = op;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendOpAlpha(BlendOperation::Code op) {
    this->BlendOpAlpha = op;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetColorWriteMask(PixelChannel::Mask m) {
    this->ColorWriteMask = m;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetColorFormat(PixelFormat::Code fmt) {
    this->ColorFormat = fmt;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetDepthFormat(PixelFormat::Code fmt) {
    this->DepthFormat = fmt;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetSampleCount(int c) {
    this->SampleCount = c;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetMRTCount(int c) {
    this->MRTCount = c;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetBlendColor(const glm::vec4& c) {
    this->BlendColor = c;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetCullFaceEnabled(bool b) {
    this->CullFaceEnabled = b;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetCullFace(Face::Code f) {
    this->CullFace = f;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetAlphaToCoverageEnabled(bool b) {
    this->AlphaToCoverageEnabled = b;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetDepthBias(float f) {
    this->DepthBias = f;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetDepthBiasSlopeScale(float f) {
    this->DepthBiasSlopeScale = f;
    return *this;
}

//------------------------------------------------------------------------------
PipelineDesc& PipelineDesc::SetDepthBiasClamp(float f) {
    this->DepthBiasClamp = f;
    return *this;
}

//------------------------------------------------------------------------------
ShaderDesc::ShaderDesc(const ShaderDesc& rhs) {
    *this = rhs;
}

//------------------------------------------------------------------------------
ShaderDesc& ShaderDesc::SetLocator(const class Locator& loc) {
    this->Locator = loc;
    return *this;
}

//------------------------------------------------------------------------------
ShaderDesc& ShaderDesc::SetSource(ShaderStage::Code stg, const char* src) {
    this->Stage[stg].Source = src;
    return *this;
}

//------------------------------------------------------------------------------
ShaderDesc& ShaderDesc::SetByteCode(ShaderStage::Code stg, const uint8_t* ptr, int size) {
    this->Stage[stg].ByteCode = ptr;
    this->Stage[stg].ByteCodeSize = size;
    return *this;
}

//------------------------------------------------------------------------------
ShaderDesc& ShaderDesc::SetEntry(ShaderStage::Code stg, const char* entry) {
    this->Stage[stg].Entry = entry; return *this;
}

//------------------------------------------------------------------------------
ShaderDesc& ShaderDesc::SetAttr(const StringAtom& name, VertexFormat::Code fmt) {
    this->Layout.Add(name, fmt);
    return *this;
}

//------------------------------------------------------------------------------
ShaderDesc& ShaderDesc::SetUniformBlock(ShaderStage::Code stg, int slot, const char* name, const char* type, int size) {
    auto& ubSlot = this->Stage[stg].UniformBlocks[slot];
    ubSlot.Name = name;
    ubSlot.Type = type;
    ubSlot.Size = size;
    return *this;
}

//------------------------------------------------------------------------------
ShaderDesc& ShaderDesc::SetTexture(ShaderStage::Code stg, int slot, const char* name, TextureType::Code type) {
    auto& texSlot = this->Stage[stg].Textures[slot];
    texSlot.Name = name;
    texSlot.Type = type;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc::TextureDesc() {
    this->NativeTextures.Fill(0);
}

//------------------------------------------------------------------------------
TextureDesc::TextureDesc(const TextureDesc& rhs) {
    *this = rhs;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetLocator(const class Locator& loc) {
    this->Locator = loc;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetType(TextureType::Code t) {
    this->Type = t;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetRenderTarget(bool b) {
    this->RenderTarget = b;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetWidth(int w) {
    this->Width = w;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetHeight(int h) {
    this->Height = h;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetDepth(int d) {
    this->Depth = d;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetLayers(int l) {
    // not a bug
    this->Depth = l;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetNumMipMaps(int n) {
    this->NumMipMaps = n;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetUsage(Usage::Code u) {
    this->Usage = u;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetFormat(PixelFormat::Code fmt) {
    this->Format = fmt;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetSampleCount(int c) {
    this->SampleCount = c;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetMagFilter(TextureFilterMode::Code f) {
    this->MagFilter = f;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetMinFilter(TextureFilterMode::Code f) {
    this->MinFilter = f;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetWrapU(TextureWrapMode::Code m) {
    this->WrapU = m;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetWrapV(TextureWrapMode::Code m) {
    this->WrapV = m;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetWrapW(TextureWrapMode::Code m) {
    this->WrapW = m;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetNativeTexture(int index, intptr_t tex) {
    this->NativeTextures[index] = tex;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetMipSize(int faceIndex, int mipIndex, int size) {
    this->Content.Size[faceIndex][mipIndex] = size;
    return *this;
}

//------------------------------------------------------------------------------
TextureDesc& TextureDesc::SetMipContent(int faceIndex, int mipIndex, const void* ptr) {
    this->Content.Pointer[faceIndex][mipIndex] = ptr;
    return *this;
}

//------------------------------------------------------------------------------
PassDesc::PassDesc(const PassDesc& rhs) {
    *this = rhs;
}

//------------------------------------------------------------------------------
PassDesc& PassDesc::SetLocator(const class Locator& loc) {
    this->Locator = loc;
    return *this;
}

//------------------------------------------------------------------------------
PassDesc& PassDesc::SetColorAttachment(int slotIndex, const Id& tex, int mipLevel, int faceLayerSlice) {
    auto& att = this->ColorAttachments[slotIndex];
    att.Texture = tex;
    att.MipLevel = mipLevel;
    att.Face = faceLayerSlice;
    return *this;
}

//------------------------------------------------------------------------------
PassDesc& PassDesc::SetDepthStencilAttachment(const Id& tex, int mipLevel, int faceLayerSlice) {
    auto& att = this->DepthStencilAttachment;
    att.Texture = tex;
    att.MipLevel = mipLevel;
    att.Face = faceLayerSlice;
    return *this;
}

} // namespace Oryol

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
PassAction::PassAction() {
    for (auto& c : this->Color) {
        c = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

//------------------------------------------------------------------------------
PassAction PassAction::New() {
    return PassAction();
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
VertexLayout VertexLayout::New() {
    return VertexLayout();
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

} // namespace Oryol

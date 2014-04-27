//------------------------------------------------------------------------------
//  RenderEnumsTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/String.h"
#include "Render/Types/PixelFormat.h"
#include "Render/Types/PrimitiveType.h"
#include "Render/Types/TextureType.h"
#include "Render/Types/Usage.h"
#include "Render/Types/TextureWrapMode.h"
#include "Render/Types/IndexType.h"
#include "Render/Types/TextureFilterMode.h"
#include "Render/Types/VertexFormat.h"
#include "Render/Types/VertexAttr.h"
#include "Render/Types/ResourceType.h"
#include "Render/Types/ShaderType.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

//------------------------------------------------------------------------------
TEST(PixelFormatTest) {
    CHECK(PixelFormat::NumPixelFormats == 15);
    
    CHECK(PixelFormat::FromString("R8G8B8A8") == PixelFormat::R8G8B8A8);
    CHECK(PixelFormat::FromString("R8G8B8") == PixelFormat::R8G8B8);
    CHECK(PixelFormat::FromString("R5G6B5") == PixelFormat::R5G6B5);
    CHECK(PixelFormat::FromString("R5G5B5A1") == PixelFormat::R5G5B5A1);
    CHECK(PixelFormat::FromString("R4G4B4A4") == PixelFormat::R4G4B4A4);
    CHECK(PixelFormat::FromString("L8") == PixelFormat::L8);
    CHECK(PixelFormat::FromString("DXT1") == PixelFormat::DXT1);
    CHECK(PixelFormat::FromString("DXT3") == PixelFormat::DXT3);
    CHECK(PixelFormat::FromString("DXT5") == PixelFormat::DXT5);
    CHECK(PixelFormat::FromString("D16") == PixelFormat::D16);
    CHECK(PixelFormat::FromString("D32") == PixelFormat::D32);
    CHECK(PixelFormat::FromString("D24S8") == PixelFormat::D24S8);
    CHECK(PixelFormat::FromString("R32F") == PixelFormat::R32F);
    CHECK(PixelFormat::FromString("PVRTC2") == PixelFormat::PVRTC2);
    CHECK(PixelFormat::FromString("PVRTC4") == PixelFormat::PVRTC4);

    CHECK(String(PixelFormat::ToString(PixelFormat::R8G8B8A8)) == "R8G8B8A8");
    CHECK(String(PixelFormat::ToString(PixelFormat::R8G8B8)) == "R8G8B8");
    CHECK(String(PixelFormat::ToString(PixelFormat::R5G6B5)) == "R5G6B5");
    CHECK(String(PixelFormat::ToString(PixelFormat::R5G5B5A1)) == "R5G5B5A1");
    CHECK(String(PixelFormat::ToString(PixelFormat::R4G4B4A4)) == "R4G4B4A4");
    CHECK(String(PixelFormat::ToString(PixelFormat::L8)) == "L8");
    CHECK(String(PixelFormat::ToString(PixelFormat::DXT1)) == "DXT1");
    CHECK(String(PixelFormat::ToString(PixelFormat::DXT3)) == "DXT3");
    CHECK(String(PixelFormat::ToString(PixelFormat::DXT5)) == "DXT5");
    CHECK(String(PixelFormat::ToString(PixelFormat::D16)) == "D16");
    CHECK(String(PixelFormat::ToString(PixelFormat::D32)) == "D32");
    CHECK(String(PixelFormat::ToString(PixelFormat::D24S8)) == "D24S8");
    CHECK(String(PixelFormat::ToString(PixelFormat::R32F)) == "R32F");
    CHECK(String(PixelFormat::ToString(PixelFormat::PVRTC2)) == "PVRTC2");
    CHECK(String(PixelFormat::ToString(PixelFormat::PVRTC4)) == "PVRTC4");
}

//------------------------------------------------------------------------------
TEST(PixelFormatChannelBitsTest) {

    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Red) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Green) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Blue) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Alpha) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8A8, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Red) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Green) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Blue) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R8G8B8, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Red) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Green) == 6);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Blue) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G6B5, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Red) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Green) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Blue) == 5);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Alpha) == 1);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R5G5B5A1, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Red) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Green) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Blue) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Alpha) == 4);
    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R4G4B4A4, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Red) == 8);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::L8, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Depth) == 16);
    CHECK(PixelFormat::NumBits(PixelFormat::D16, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Depth) == 32);
    CHECK(PixelFormat::NumBits(PixelFormat::D32, PixelFormat::Stencil) == 0);

    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Red) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Depth) == 24);
    CHECK(PixelFormat::NumBits(PixelFormat::D24S8, PixelFormat::Stencil) == 8);

    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Red) == 32);
    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Green) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Blue) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Alpha) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Depth) == 0);
    CHECK(PixelFormat::NumBits(PixelFormat::R32F, PixelFormat::Stencil) == 0);

    // all other pixel formats must return 0 for all channels
    for (int pf = 0; pf < PixelFormat::NumPixelFormats; pf++) {
        if ((pf != PixelFormat::R8G8B8A8) &&
            (pf != PixelFormat::R8G8B8) &&
            (pf != PixelFormat::R5G6B5) &&
            (pf != PixelFormat::R5G5B5A1) &&
            (pf != PixelFormat::R4G4B4A4) &&
            (pf != PixelFormat::L8) &&
            (pf != PixelFormat::D16) &&
            (pf != PixelFormat::D32) &&
            (pf != PixelFormat::D24S8) &&
            (pf != PixelFormat::R32F)) {
            for (int chn = 0; chn < PixelFormat::NumChannels; chn++) {
                CHECK(PixelFormat::NumBits((PixelFormat::Code)pf, (PixelFormat::Channel)chn) == 0);
            }
        }
    }
}

//------------------------------------------------------------------------------
TEST(TextureTypeTest) {
    CHECK(TextureType::NumTextureTypes == 3);
    CHECK(TextureType::FromString("Texture2D") == TextureType::Texture2D);
    CHECK(TextureType::FromString("Texture3D") == TextureType::Texture3D);
    CHECK(TextureType::FromString("TextureCube") == TextureType::TextureCube);
    CHECK(TextureType::FromString("Bla") == TextureType::InvalidTextureType);
    CHECK(String(TextureType::ToString(TextureType::Texture2D)) == "Texture2D");
    CHECK(String(TextureType::ToString(TextureType::Texture3D)) == "Texture3D");
    CHECK(String(TextureType::ToString(TextureType::TextureCube)) == "TextureCube");
}

//------------------------------------------------------------------------------
TEST(PrimitiveTypeTest) {
    CHECK(PrimitiveType::NumPrimitiveTypes == 7);
    
    CHECK(PrimitiveType::FromString("Points") == PrimitiveType::Points);
    CHECK(PrimitiveType::FromString("LineStrip") == PrimitiveType::LineStrip);
    CHECK(PrimitiveType::FromString("LineLoop") == PrimitiveType::LineLoop);
    CHECK(PrimitiveType::FromString("Lines") == PrimitiveType::Lines);
    CHECK(PrimitiveType::FromString("TriangleStrip") == PrimitiveType::TriangleStrip);
    CHECK(PrimitiveType::FromString("TriangleFan") == PrimitiveType::TriangleFan);
    CHECK(PrimitiveType::FromString("Triangles") == PrimitiveType::Triangles);
    
    CHECK(String(PrimitiveType::ToString(PrimitiveType::Points)) == "Points");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::LineStrip)) == "LineStrip");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::LineLoop)) == "LineLoop");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::Lines)) == "Lines");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::TriangleStrip)) == "TriangleStrip");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::TriangleFan)) == "TriangleFan");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::Triangles)) == "Triangles");
}

//------------------------------------------------------------------------------
TEST(UsageTest) {
    CHECK(Usage::NumUsages == 3);
    CHECK(Usage::FromString("Immutable") == Usage::Immutable);
    CHECK(Usage::FromString("DynamicWrite") == Usage::DynamicWrite);
    CHECK(Usage::FromString("DynamicStream") == Usage::DynamicStream);
    CHECK(Usage::FromString("Bla") == Usage::InvalidUsage);
    CHECK(String(Usage::ToString(Usage::Immutable)) == "Immutable");
    CHECK(String(Usage::ToString(Usage::DynamicWrite)) == "DynamicWrite");
    CHECK(String(Usage::ToString(Usage::DynamicStream)) == "DynamicStream");
}

//------------------------------------------------------------------------------
TEST(TextureWrapMode) {
    CHECK(TextureWrapMode::NumTextureWrapModes == 3);
    CHECK(TextureWrapMode::FromString("ClampToEdge") == TextureWrapMode::ClampToEdge);
    CHECK(TextureWrapMode::FromString("Repeat") == TextureWrapMode::Repeat);
    CHECK(TextureWrapMode::FromString("MirroredRepeat") == TextureWrapMode::MirroredRepeat);
    CHECK(String(TextureWrapMode::ToString(TextureWrapMode::ClampToEdge)) == "ClampToEdge");
    CHECK(String(TextureWrapMode::ToString(TextureWrapMode::Repeat)) == "Repeat");
    CHECK(String(TextureWrapMode::ToString(TextureWrapMode::MirroredRepeat)) == "MirroredRepeat");
}

//------------------------------------------------------------------------------
TEST(IndexTypeTest) {
    CHECK(IndexType::NumIndexTypes == 3);
    CHECK(IndexType::FromString("None") == IndexType::None);
    CHECK(IndexType::FromString("Index16") == IndexType::Index16);
    CHECK(IndexType::FromString("Index32") == IndexType::Index32);
    CHECK(String(IndexType::ToString(IndexType::None)) == "None");
    CHECK(String(IndexType::ToString(IndexType::Index16)) == "Index16");
    CHECK(String(IndexType::ToString(IndexType::Index32)) == "Index32");
    CHECK(IndexType::ByteSize(IndexType::Index16) == 2);
    CHECK(IndexType::ByteSize(IndexType::Index32) == 4);
}

//------------------------------------------------------------------------------
TEST(TextureFilterModeTest) {
    CHECK(TextureFilterMode::NumTextureFilterModes == 6);
    CHECK(TextureFilterMode::FromString("Nearest") == TextureFilterMode::Nearest);
    CHECK(TextureFilterMode::FromString("Linear") == TextureFilterMode::Linear);
    CHECK(TextureFilterMode::FromString("NearestMipmapNearest") == TextureFilterMode::NearestMipmapNearest);
    CHECK(TextureFilterMode::FromString("NearestMipmapLinear") == TextureFilterMode::NearestMipmapLinear);
    CHECK(TextureFilterMode::FromString("LinearMipmapNearest") == TextureFilterMode::LinearMipmapNearest);
    CHECK(TextureFilterMode::FromString("LinearMipmapLinear") == TextureFilterMode::LinearMipmapLinear);
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::Nearest)) == "Nearest");
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::Linear)) == "Linear");
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::NearestMipmapNearest)) == "NearestMipmapNearest");
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::NearestMipmapLinear)) == "NearestMipmapLinear");
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::LinearMipmapNearest)) == "LinearMipmapNearest");
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::LinearMipmapLinear)) == "LinearMipmapLinear");
}

//------------------------------------------------------------------------------
TEST(VertexFormatTest) {
    CHECK(VertexFormat::NumVertexFormats == 12);
    
    CHECK(VertexFormat::FromString("Float") == VertexFormat::Float);
    CHECK(VertexFormat::FromString("Float2") == VertexFormat::Float2);
    CHECK(VertexFormat::FromString("Float3") == VertexFormat::Float3);
    CHECK(VertexFormat::FromString("Float4") == VertexFormat::Float4);
    CHECK(VertexFormat::FromString("Byte4") == VertexFormat::Byte4);
    CHECK(VertexFormat::FromString("Byte4N") == VertexFormat::Byte4N);
    CHECK(VertexFormat::FromString("UByte4") == VertexFormat::UByte4);
    CHECK(VertexFormat::FromString("UByte4N") == VertexFormat::UByte4N);
    CHECK(VertexFormat::FromString("Short2") == VertexFormat::Short2);
    CHECK(VertexFormat::FromString("Short2N") == VertexFormat::Short2N);
    CHECK(VertexFormat::FromString("Short4") == VertexFormat::Short4);
    CHECK(VertexFormat::FromString("Short4N") == VertexFormat::Short4N);
    
    CHECK(String(VertexFormat::ToString(VertexFormat::Float)) == "Float");
    CHECK(String(VertexFormat::ToString(VertexFormat::Float2)) == "Float2");
    CHECK(String(VertexFormat::ToString(VertexFormat::Float3)) == "Float3");
    CHECK(String(VertexFormat::ToString(VertexFormat::Float4)) == "Float4");
    CHECK(String(VertexFormat::ToString(VertexFormat::Byte4)) == "Byte4");
    CHECK(String(VertexFormat::ToString(VertexFormat::Byte4N)) == "Byte4N");
    CHECK(String(VertexFormat::ToString(VertexFormat::UByte4)) == "UByte4");
    CHECK(String(VertexFormat::ToString(VertexFormat::UByte4N)) == "UByte4N");
    CHECK(String(VertexFormat::ToString(VertexFormat::Short2)) == "Short2");
    CHECK(String(VertexFormat::ToString(VertexFormat::Short2N)) == "Short2N");
    CHECK(String(VertexFormat::ToString(VertexFormat::Short4)) == "Short4");
    CHECK(String(VertexFormat::ToString(VertexFormat::Short4N)) == "Short4N");
    
    CHECK(VertexFormat::ByteSize(VertexFormat::Float) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Float2) == 8);
    CHECK(VertexFormat::ByteSize(VertexFormat::Float3) == 12);
    CHECK(VertexFormat::ByteSize(VertexFormat::Float4) == 16);
    CHECK(VertexFormat::ByteSize(VertexFormat::Byte4) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Byte4N) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::UByte4) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::UByte4N) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short2) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short2N) == 4);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short4) == 8);
    CHECK(VertexFormat::ByteSize(VertexFormat::Short4N) == 8);
}

//------------------------------------------------------------------------------
TEST(VertexAttrTest) {
    CHECK(VertexAttr::NumVertexAttrs == 16);
    
    CHECK(VertexAttr::FromString("position") == VertexAttr::Position);
    CHECK(VertexAttr::FromString("normal") == VertexAttr::Normal);
    CHECK(VertexAttr::FromString("texcoord0") == VertexAttr::TexCoord0);
    CHECK(VertexAttr::FromString("texcoord1") == VertexAttr::TexCoord1);
    CHECK(VertexAttr::FromString("texcoord2") == VertexAttr::TexCoord2);
    CHECK(VertexAttr::FromString("texcoord3") == VertexAttr::TexCoord3);
    CHECK(VertexAttr::FromString("tangent") == VertexAttr::Tangent);
    CHECK(VertexAttr::FromString("binormal") == VertexAttr::Binormal);
    CHECK(VertexAttr::FromString("weights") == VertexAttr::Weights);
    CHECK(VertexAttr::FromString("indices") == VertexAttr::Indices);
    CHECK(VertexAttr::FromString("color0") == VertexAttr::Color0);
    CHECK(VertexAttr::FromString("color1") == VertexAttr::Color1);
    CHECK(VertexAttr::FromString("custom0") == VertexAttr::Custom0);
    CHECK(VertexAttr::FromString("custom1") == VertexAttr::Custom1);
    CHECK(VertexAttr::FromString("custom2") == VertexAttr::Custom2);
    CHECK(VertexAttr::FromString("custom3") == VertexAttr::Custom3);
    
    CHECK(String(VertexAttr::ToString(VertexAttr::Position)) == "position");
    CHECK(String(VertexAttr::ToString(VertexAttr::Normal)) == "normal");
    CHECK(String(VertexAttr::ToString(VertexAttr::TexCoord0)) == "texcoord0");
    CHECK(String(VertexAttr::ToString(VertexAttr::TexCoord1)) == "texcoord1");
    CHECK(String(VertexAttr::ToString(VertexAttr::TexCoord2)) == "texcoord2");
    CHECK(String(VertexAttr::ToString(VertexAttr::TexCoord3)) == "texcoord3");
    CHECK(String(VertexAttr::ToString(VertexAttr::Tangent)) == "tangent");
    CHECK(String(VertexAttr::ToString(VertexAttr::Binormal)) == "binormal");
    CHECK(String(VertexAttr::ToString(VertexAttr::Weights)) == "weights");
    CHECK(String(VertexAttr::ToString(VertexAttr::Indices)) == "indices");
    CHECK(String(VertexAttr::ToString(VertexAttr::Color0)) == "color0");
    CHECK(String(VertexAttr::ToString(VertexAttr::Color1)) == "color1");
    CHECK(String(VertexAttr::ToString(VertexAttr::Custom0)) == "custom0");
    CHECK(String(VertexAttr::ToString(VertexAttr::Custom1)) == "custom1");
    CHECK(String(VertexAttr::ToString(VertexAttr::Custom2)) == "custom2");
    CHECK(String(VertexAttr::ToString(VertexAttr::Custom3)) == "custom3");
}

//------------------------------------------------------------------------------
TEST(RenderResourceTypeTest) {
    CHECK(ResourceType::NumResourceTypes == 6);
    
    CHECK(ResourceType::FromString("Texture") == ResourceType::Texture);
    CHECK(ResourceType::FromString("Mesh") == ResourceType::Mesh);
    CHECK(ResourceType::FromString("Shader") == ResourceType::Shader);
    CHECK(ResourceType::FromString("ProgramBundle") == ResourceType::ProgramBundle);
    CHECK(ResourceType::FromString("StateBlock") == ResourceType::StateBlock);
    CHECK(ResourceType::FromString("ConstantBlock") == ResourceType::ConstantBlock);

    CHECK(String(ResourceType::ToString(ResourceType::Texture)) == "Texture");
    CHECK(String(ResourceType::ToString(ResourceType::Mesh)) == "Mesh");
    CHECK(String(ResourceType::ToString(ResourceType::Shader)) == "Shader");
    CHECK(String(ResourceType::ToString(ResourceType::ProgramBundle)) == "ProgramBundle");
    CHECK(String(ResourceType::ToString(ResourceType::StateBlock)) == "StateBlock");
    CHECK(String(ResourceType::ToString(ResourceType::ConstantBlock)) == "ConstantBlock");
}

//------------------------------------------------------------------------------
TEST(ShaderTypeTest) {
    CHECK(ShaderType::NumShaderTypes == 2);
    
    CHECK(ShaderType::FromString("VertexShader") == ShaderType::VertexShader);
    CHECK(ShaderType::FromString("FragmentShader") == ShaderType::FragmentShader);
    
    CHECK(String(ShaderType::ToString(ShaderType::VertexShader)) == "VertexShader");
    CHECK(String(ShaderType::ToString(ShaderType::FragmentShader)) == "FragmentShader");
}

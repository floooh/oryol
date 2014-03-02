//------------------------------------------------------------------------------
//  TextureTypeTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/TextureType.h"
#include "Core/String/String.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

TEST(TextureTypeTest) {
    CHECK(TextureType::NumTextureTypes == 4);
    CHECK(TextureType::FromString("Texture1D") == TextureType::Texture1D);
    CHECK(TextureType::FromString("Texture2D") == TextureType::Texture2D);
    CHECK(TextureType::FromString("Texture3D") == TextureType::Texture3D);
    CHECK(TextureType::FromString("TextureCube") == TextureType::TextureCube);
    CHECK(TextureType::FromString("Bla") == TextureType::InvalidTextureType);
    CHECK(String(TextureType::ToString(TextureType::Texture1D)) == "Texture1D");
    CHECK(String(TextureType::ToString(TextureType::Texture2D)) == "Texture2D");
    CHECK(String(TextureType::ToString(TextureType::Texture3D)) == "Texture3D");
    CHECK(String(TextureType::ToString(TextureType::TextureCube)) == "TextureCube");
}

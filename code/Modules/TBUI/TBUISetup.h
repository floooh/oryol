#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TBUISetup
    @brief setup parameters for the TBUI module
*/
#include "Core/Types.h"
#include "IO/Core/URL.h"
#include "TBUI/TBUIFontInfo.h"

namespace Oryol {

class TBUISetup {
public:
    /// language file resource
    URL Locale;
    /// default skin resource
    URL DefaultSkin;
    /// override skin resource
    URL OverrideSkin;
    /// font infos
    Array<TBUIFontInfo> Fonts;
    /// default font name (must be one from 'Fonts')
    String DefaultFontName;
    /// default font size
    int32 DefaultFontSize = 14;
    /// initial glyph set to prevent excessive reconstruction of font texture
    StringAtom GlyphSet = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~•·";

    /// resource pool size
    int32 ResourcePoolSize = 256;
    /// initial resource label stack capacity
    int32 ResourceLabelStackCapacity = 256;
    /// initial resource registry capacity
    int32 ResourceRegistryCapacity = 256;
};

} // namespace Oryol
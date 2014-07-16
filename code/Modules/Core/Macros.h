#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Macros.h
    @brief various low-level helper macros
*/

/// to-string / from-string helper macros
#define __ORYOL_FROMSTRING(code) if (0 == std::strcmp(#code, str)) return code
#define __ORYOL_TOSTRING(code) case code: return #code

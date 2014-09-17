#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glDebugOutput
    @ingroup _priv
    @brief detailed debugging output using ARB_debug_output
*/
#include "Core/Types.h"

namespace Oryol {
namespace _priv {

class glDebugOutput {
public:
    /// message severity
    enum Severity {
        High,
        Medium,
        Low,
    };
    
    /// enable debug output
    static void Enable(Severity s);
    /// disable debug output
    static void Disable();
    /// return true if debug output is enabled
    static bool IsEnabled();

private:
    static bool isEnabled;
};

} // namespace _priv
} // namespace Oryol
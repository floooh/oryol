#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::ioPointers
    @ingroup _priv
    @brief grant access to central IO module objects
*/
#include "Core/Types.h"

namespace Oryol {
namespace _priv {

class assignRegistry;
class schemeRegistry;

struct ioPointers {
    class assignRegistry* assignRegistry;
    class schemeRegistry* schemeRegistry;
};

} // namespace _priv
} // namespace Oryol

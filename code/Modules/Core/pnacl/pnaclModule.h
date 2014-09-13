#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pnaclModule
    @ingroup _priv
    @brief derived PP::Module class for PNaCl port
*/
#include "ppapi/cpp/module.h"

namespace Oryol {
namespace _priv {

class pnaclModule : public pp::Module {
public:
    /// return a pp::Instance object
    virtual pp::Instance* CreateInstance(PP_Instance instance);
};

} // namespace _priv
} // namespace Oryol
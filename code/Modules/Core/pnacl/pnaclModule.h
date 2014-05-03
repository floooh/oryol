#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::pnaclModule
    @brief derived PP::Module class for PNaCl port
*/
#include "ppapi/cpp/module.h"

namespace Oryol {
namespace Core {

class pnaclModule : public pp::Module {
public:
    /// return a pp::Instance object
    virtual pp::Instance* CreateInstance(PP_Instance instance);
};

} // namespace Core
} // namespace Oryol
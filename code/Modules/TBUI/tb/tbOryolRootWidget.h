#pragma once
//-----------------------------------------------------------------------------
/**
    @class Oryol::_priv::tbOryolRootWidget
    @brief Oryol implementation of turbobadger root widget
*/
#include "Core/Types.h"
#include "tb_widgets.h"

namespace Oryol {
namespace _priv {

class tbOryolRootWidget : public tb::TBWidget {
public:
    virtual void OnInvalid();
};

} // namespace _priv
} // namespace Oryol

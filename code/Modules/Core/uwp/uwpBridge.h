#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::uwpBridge
    @ingroup _priv
    @brief app bridge class for UWP platform
*/
#include "Core/Types.h"
#include "Core/Macros.h"

namespace Oryol {
class App;
namespace _priv {

class uwpBridge {
public:
    /// constructor
    uwpBridge();
    /// destructor
    ~uwpBridge();
    /// return pointer to global uwpBridge instance
    static uwpBridge* ptr();

    /// setup the app bridge
    void setup(App* app);
    /// discard the app bridge
    void discard();
    /// setup the UWP app and start main loop
    void startMainLoop();

    static uwpBridge* self;
    App* app;
};

} // namespace _priv
} // namespace Oryol
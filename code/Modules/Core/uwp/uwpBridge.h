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
    /// setup, run and discard the Oryol app (called from main())
    void start(App*(*app_creator)());

    /// create the Oryol App object (called from UI thread)
    void createApp();
    /// run one Oryol App frame
    void onFrame();
    /// discard the Oryol app
    void destroyApp();

    static uwpBridge* self;
    App* app;
    App* (*create_app)();
};

} // namespace _priv
} // namespace Oryol
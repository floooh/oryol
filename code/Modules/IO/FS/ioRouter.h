#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::ioRouter
    @ingroup IO
    @brief route IO requests to ioWorkers
*/
#include "Core/Containers/StaticArray.h"
#include "IO/Core/IOConfig.h"
#include "IO/Core/ioPointers.h"
#include "IO/FS/ioWorker.h"

namespace Oryol {
namespace _priv {

class ioRouter {
public:
    /// setup the router
    void setup(const ioPointers& ptrs);
    /// discard the router
    void discard();
    /// route a ioMsg to one or more workers
    void put(const Ptr<ioMsg>& msg);
    /// perform per-frame work
    void doWork();

private:
    int curWorker = 0;
    StaticArray<ioWorker, IOConfig::NumWorkers> workers;
};

} // namespace _priv
} // namespace Oryol


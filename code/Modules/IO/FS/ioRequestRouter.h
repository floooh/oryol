#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::ioRequestRouter
    @ingroup _priv
    @brief front end router port of the IO system
    
    @todo: ioRequestRouter description
*/
#include "IO/Core/IOConfig.h"
#include "Messaging/Port.h"
#include "IO/FS/ioLane.h"

namespace Oryol {
namespace _priv {

class ioRequestRouter : public Port {
    OryolClassDecl(ioRequestRouter);
public:
    /// constructor
    ioRequestRouter(int32 numLanes);
    /// destructor
    ~ioRequestRouter();
    
    /// put a message into the port
    virtual bool Put(const Ptr<Message>& msg) override;
    /// perform work, this will be invoked on downstream ports
    virtual void DoWork() override;
    
private:
    int32 numLanes;
    Array<Ptr<ioLane>> ioLanes;
};
    
} // namespace IO
} // namespace Oryol
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
#include "IO/Core/ioPointers.h"

namespace Oryol {
namespace _priv {

class ioRequestRouter : public Port {
    OryolClassDecl(ioRequestRouter);
public:
    /// constructor
    ioRequestRouter(int32 numLanes, const ioPointers& pointers);
    /// destructor
    ~ioRequestRouter();
    
    /// put a message into the port
    virtual bool Put(const Ptr<Message>& msg) override;
    /// pump the ioLane message queues
    virtual void DoWork() override;

private:
    int32 curLane;
    int32 numLanes;
    Array<Ptr<ioLane>> ioLanes;
};
    
} // namespace IO
} // namespace Oryol
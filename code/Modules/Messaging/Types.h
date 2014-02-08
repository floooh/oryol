#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::Types
    @brief type definitions for messaging modules
*/
#include "Core/Types.h"

namespace Oryol {
namespace Messaging {
    
typedef uint32 ProtocolIdType;
static const ProtocolIdType InvalidProtocolId = 0xFFFFFFFF;
typedef int32 MessageIdType;
static const MessageIdType InvalidMessageId = -1;
        
} // namespace Messaging
} // namespace Oryol
#pragma once
//------------------------------------------------------------------------------
/**
    @file Messaging/Types.h
    @ingroup Messaging
    @brief type definitions for messaging modules
*/
#include "Core/Types.h"

namespace Oryol {
    
typedef uint32 ProtocolIdType;
static const ProtocolIdType InvalidProtocolId = 0xFFFFFFFF;
typedef int32 MessageIdType;
static const MessageIdType InvalidMessageId = -1;
        
} // namespace Oryol
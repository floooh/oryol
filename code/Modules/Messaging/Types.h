#pragma once
//------------------------------------------------------------------------------
/**
    @file Messaging/Types.h
    @ingroup Messaging
    @brief type definitions for messaging modules
*/
#include "Core/Types.h"

namespace Oryol {
    
typedef uint32_t ProtocolIdType;
static const ProtocolIdType InvalidProtocolId = 0xFFFFFFFF;
typedef int32_t MessageIdType;
static const MessageIdType InvalidMessageId = -1;
        
} // namespace Oryol
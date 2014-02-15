#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HTTP::winURLLoader
    @brief private: Windows implementation of URLLoader
    @see urlLoader HTTPClient
 */
#include "base/baseURLLoader.h"

namespace Oryol {
namespace HTTP {
    
class winURLLoader : public baseURLLoader {
public:
    /// process enqueued requests
    void doWork();
};
    
} // namespace HTTP
} // namespace Oryol
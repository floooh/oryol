//------------------------------------------------------------------------------
//  osxURLLoader.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "osxURLLoader.h"
#include <Foundation/Foundation.h>
#include <atomic>

namespace Oryol {
namespace _priv {

std::atomic_flag osxURLLoader_sharedCacheSet{false};

//------------------------------------------------------------------------------
osxURLLoader::osxURLLoader() {
    // disable the 'magic cache' to prevent memory growth
    #if !ORYOL_USE_ARC
    if (!osxURLLoader_sharedCacheSet.test_and_set()) {
        [NSURLCache setSharedURLCache:[[[NSURLCache alloc] initWithMemoryCapacity:0
         diskCapacity:0 diskPath:nil] autorelease]];
    }
    #endif
}

//------------------------------------------------------------------------------
bool
osxURLLoader::doRequest(const Ptr<IORead>& req) {
    if (baseURLLoader::doRequest(req)) {
        this->doRequestInternal(req);
        req->Handled = true;
        return true;
    }
    else {
        // request was cancelled
        return false;
    }
}

//------------------------------------------------------------------------------
void
osxURLLoader::doRequestInternal(const Ptr<IORead>& req) {
    @autoreleasepool {
        
        // build an URL request
        NSString* urlString = [NSString stringWithUTF8String: req->Url.AsCStr()];
        NSMutableURLRequest* urlRequest = [[NSMutableURLRequest alloc] init];
        NSURL* url = [NSURL URLWithString:urlString];
        [urlRequest setURL:url];
        [urlRequest setHTTPMethod:@"GET"];
        #if ORYOL_DEBUG
        [urlRequest setCachePolicy:NSURLRequestReloadIgnoringLocalCacheData];
        #endif
        
        // add standard request headers:
        //  User-Agent: need a 'standard' user-agent, otherwise some HTTP servers
        //              won't accept Connection: keep-alive
        //  Connection: keep-alive, don't open/close the connection all the time
        //  Accept-Encoding:    gzip, deflate
        //
        // FIXME: is this actually necessary on iOS?
        [urlRequest setValue:@"Mozilla/5.0" forHTTPHeaderField:@"User-Agent"];
        [urlRequest setValue:@"keep-alive" forHTTPHeaderField:@"Connection"];
        [urlRequest setValue:@"gzip, deflate" forHTTPHeaderField:@"Accept-Encoding"];

        // now perform a synchronous request
        NSHTTPURLResponse* urlResponse = nil;
        NSError* urlError = nil;
        NSData* responseData = [NSURLConnection sendSynchronousRequest:urlRequest returningResponse:&urlResponse error:&urlError];
        if (nil != responseData) {
            o_assert(nil != urlResponse);
            
            // extract HTTP status...
            req->Status = (IOStatus::Code) [urlResponse statusCode];
            
            // extract response body...
            const uint8_t* responseBytes = (const uint8_t*) [responseData bytes];
            const int responseLength = (const int) [responseData length];
            if (responseLength > 0) {
                req->Data.Add(responseBytes, responseLength);
            }
        }
        else {
            // an error occurred
            IOStatus::Code ioStatus = IOStatus::InvalidIOStatus;
            if (nil != urlResponse) {
                ioStatus = (IOStatus::Code) [urlResponse statusCode];
            }
            req->Status = (IOStatus::Code) ioStatus;
            if (nil != urlError) {
                req->ErrorDesc = [[urlError localizedDescription] UTF8String];
            }
        }
        #if !ORYOL_USE_ARC
        [urlRequest autorelease];
        #endif
    }
}

} // namespace _priv
} // namespace Oryol

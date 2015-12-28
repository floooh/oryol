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
osxURLLoader::doRequest(const Ptr<HTTPProtocol::HTTPRequest>& httpReq) {
    if (baseURLLoader::doRequest(httpReq)) {
        this->doRequestInternal(httpReq);
        // transfer result to embedded IoRequest and set to handled
        const Ptr<IOProtocol::Request>& ioReq = httpReq->IoRequest;
        if (ioReq) {
            const Ptr<HTTPProtocol::HTTPResponse>& httpResponse = httpReq->Response;
            ioReq->Status = httpResponse->Status;
            ioReq->Data = std::move(httpResponse->Body);
            ioReq->Type = httpResponse->Type;
            ioReq->ErrorDesc = httpResponse->ErrorDesc;
            ioReq->SetHandled();
        }
        httpReq->SetHandled();
        return true;
    }
    else {
        // request was cancelled
        return false;
    }
}

//------------------------------------------------------------------------------
void
osxURLLoader::doRequestInternal(const Ptr<HTTPProtocol::HTTPRequest>& req) {
    @autoreleasepool {
        
        // build an URL request
        NSString* methodString = [NSString stringWithUTF8String: HTTPMethod::ToString(req->Method)];
        NSString* urlString = [NSString stringWithUTF8String: req->Url.AsCStr()];
        NSMutableURLRequest* urlRequest = [[NSMutableURLRequest alloc] init];
        NSURL* url = [NSURL URLWithString:urlString];
        [urlRequest setURL:url];
        [urlRequest setHTTPMethod:methodString];
        #if ORYOL_DEBUG
        [urlRequest setCachePolicy:NSURLRequestReloadIgnoringLocalCacheData];
        #endif
        
        // additional header fields
        for (const auto& field : req->RequestHeaders) {
            NSString* name  = [NSString stringWithUTF8String: field.Key().AsCStr()];
            NSString* value = [NSString stringWithUTF8String: field.Value().AsCStr()];
            [urlRequest setValue:value forHTTPHeaderField:name];
        }
        
        // optional content body
        if (!req->Body.Empty()) {
        
            // add Content-Length field
            const int32 bodySize = req->Body.Size();
            o_assert(bodySize > 0);
            NSString* contentLengthString = [NSString stringWithFormat:@"%d", bodySize];
            [urlRequest setValue:contentLengthString forHTTPHeaderField:@"Content-Length"];
            
            // add Content-Type field
            if (req->Type.IsValid()) {
                const ContentType& contentType = req->Type;
                NSString* contentTypeString = [NSString stringWithUTF8String: contentType.AsCStr()];
                [urlRequest setValue:contentTypeString forHTTPHeaderField:@"Content-Type"];
            }
            
            // add the body data
            const uint8* bodyDataPtr = req->Body.Data();
            o_assert(bodyDataPtr);
            NSData* bodyData = [NSData dataWithBytes:bodyDataPtr length:bodySize];
            [urlRequest setHTTPBody:bodyData];
        }
        
        // now perform a synchronous request
        NSHTTPURLResponse* urlResponse = nil;
        NSError* urlError = nil;
        NSData* responseData = [NSURLConnection sendSynchronousRequest:urlRequest returningResponse:&urlResponse error:&urlError];
        if (nil != responseData) {
            o_assert(nil != urlResponse);
            
            Ptr<HTTPProtocol::HTTPResponse> response = HTTPProtocol::HTTPResponse::Create();
            ContentType responseContentType;
            
            // extract HTTP status...
            response->Status = (IOStatus::Code) [urlResponse statusCode];
            
            // extract response header fields...
            NSDictionary* headerFields = [urlResponse allHeaderFields];
            Map<String,String> fields;
            for (id key in headerFields) {
                String keyString = [key UTF8String];
                String valString = [[headerFields objectForKey:key] UTF8String];
                fields.Add(keyString, valString);
                if (keyString == "Content-Type") {
                    responseContentType = valString;
                }
            }
            response->ResponseHeaders = fields;
            
            // extract response body...
            const uint8* responseBytes = (const uint8*) [responseData bytes];
            const int responseLength = (const int) [responseData length];
            if (responseLength > 0) {
                response->Body.Add(responseBytes, responseLength);
                if (responseContentType.IsValid()) {
                    response->Type = responseContentType;
                }
            }
            req->Response = response;
        }
        else {
            // an error occurred
            IOStatus::Code ioStatus = IOStatus::InvalidIOStatus;
            if (nil != urlResponse) {
                ioStatus = (IOStatus::Code) [urlResponse statusCode];
            }
            Ptr<HTTPProtocol::HTTPResponse> response = HTTPProtocol::HTTPResponse::Create();
            response->Status = (IOStatus::Code) ioStatus;
            if (nil != urlError) {
                response->ErrorDesc = [[urlError localizedDescription] UTF8String];
            }
            req->Response = response;
        }
        #if !ORYOL_USE_ARC
        [urlRequest autorelease];
        #endif
    }
}

} // namespace _priv
} // namespace Oryol
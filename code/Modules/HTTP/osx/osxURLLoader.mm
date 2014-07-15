//------------------------------------------------------------------------------
//  osxURLLoader.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "osxURLLoader.h"
#include "IO/Stream/MemoryStream.h"
#include <Foundation/Foundation.h>

namespace Oryol {
namespace HTTP {

using namespace Core;
using namespace IO;

//------------------------------------------------------------------------------
void
osxURLLoader::doWork() {
    while (!this->requestQueue.Empty()) {
        Ptr<HTTPProtocol::HTTPRequest> req = this->requestQueue.Dequeue();
        this->doOneRequest(req);
        req->SetHandled();
    }
}

//------------------------------------------------------------------------------
void
osxURLLoader::doOneRequest(const Ptr<HTTPProtocol::HTTPRequest>& req) {
    @autoreleasepool {
        
        // build an URL request
        NSString* methodString = [NSString stringWithUTF8String: HTTPMethod::ToString(req->GetMethod())];
        NSString* urlString = [NSString stringWithUTF8String: req->GetURL().AsCStr()];
        NSMutableURLRequest* urlRequest = [[NSMutableURLRequest alloc] init];
        NSURL* url = [NSURL URLWithString:urlString];
        [urlRequest setURL:url];
        [urlRequest setHTTPMethod:methodString];
        #if ORYOL_DEBUG
        [urlRequest setCachePolicy:NSURLRequestReloadIgnoringLocalCacheData];
        #endif
        
        // additional header fields
        for (const auto& field : req->GetRequestHeaders()) {
            NSString* name  = [NSString stringWithUTF8String: field.Key().AsCStr()];
            NSString* value = [NSString stringWithUTF8String: field.Value().AsCStr()];
            [urlRequest setValue:value forHTTPHeaderField:name];
        }
        
        // optional content body
        if (req->GetBody().isValid()) {
        
            // add Content-Length field
            const Ptr<Stream>& bodyStream = req->GetBody();
            const int32 bodySize = bodyStream->Size();
            o_assert(bodySize > 0);
            NSString* contentLengthString = [NSString stringWithFormat:@"%d", bodySize];
            [urlRequest setValue:contentLengthString forHTTPHeaderField:@"Content-Length"];
            
            // add Content-Type field
            if (bodyStream->GetContentType().IsValid()) {
                const ContentType& contentType = bodyStream->GetContentType();
                NSString* contentTypeString = [NSString stringWithUTF8String: contentType.AsCStr()];
                [urlRequest setValue:contentTypeString forHTTPHeaderField:@"Content-Type"];
            }
            
            // add the body data
            bodyStream->Open(OpenMode::ReadOnly);
            const uint8* maxValidPtr = nullptr;
            const uint8* bodyDataPtr = bodyStream->MapRead(&maxValidPtr);
            o_assert((0 != bodyDataPtr) && ((bodyDataPtr + bodySize) == maxValidPtr));
            NSData* bodyData = [NSData dataWithBytes:bodyDataPtr length:bodySize];
            bodyStream->UnmapRead();
            bodyStream->Close();
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
            response->SetStatus((IOStatus::Code) [urlResponse statusCode]);
            
            // extract response header fields...
            NSDictionary* headerFields = [urlResponse allHeaderFields];
            Map<String,String> fields;
            for (id key in headerFields) {
                String keyString = [key UTF8String];
                String valString = [[headerFields objectForKey:key] UTF8String];
                fields.Insert(keyString, valString);
                if (keyString == "Content-Type") {
                    responseContentType = valString;
                }
            }
            response->SetResponseHeaders(fields);
            
            // extract response body...
            const void* responseBytes = [responseData bytes];
            const int responseLength = [responseData length];
            if (responseLength > 0) {
                Ptr<MemoryStream> responseBody = MemoryStream::Create();
                responseBody->Open(OpenMode::WriteOnly);
                responseBody->Write(responseBytes, responseLength);
                responseBody->Close();
                if (responseContentType.IsValid()) {
                    responseBody->SetContentType(responseContentType);
                }
                response->SetBody(responseBody);
            }
            req->SetResponse(response);
        }
        else {
            // an error occurred
            Ptr<HTTPProtocol::HTTPResponse> response = HTTPProtocol::HTTPResponse::Create();
            if (nil != urlResponse) {
                response->SetStatus((IOStatus::Code) [urlResponse statusCode]);
            }
            if (nil != urlError) {
                response->SetErrorDesc([[urlError localizedDescription] UTF8String]);
            }
            req->SetResponse(response);
        }
        [urlRequest autorelease];
    }
}

} // namespace HTTP
} // namespace Oryol
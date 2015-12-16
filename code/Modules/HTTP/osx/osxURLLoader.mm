//------------------------------------------------------------------------------
//  osxURLLoader.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "osxURLLoader.h"
#include "IO/Stream/MemoryStream.h"
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
void
osxURLLoader::doWork() {

    while (!this->requestQueue.Empty()) {
        // don't process any cancelled requests
        Ptr<HTTPProtocol::HTTPRequest> httpReq = this->requestQueue.Dequeue();
        if (!baseURLLoader::handleCancelled(httpReq)) {
            // process the request
            this->doOneRequest(httpReq);
            
            // transfer result to embedded IoRequest and set to handled
            auto ioReq = httpReq->IoRequest;
            if (ioReq) {
                auto httpResponse = httpReq->Response;
                ioReq->Status = httpResponse->Status;
                ioReq->Data = httpResponse->Body;
                ioReq->ErrorDesc = httpResponse->ErrorDesc;
                ioReq->SetHandled();
            }
            httpReq->SetHandled();
        }
    }
}

//------------------------------------------------------------------------------
void
osxURLLoader::doOneRequest(const Ptr<HTTPProtocol::HTTPRequest>& req) {
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
        if (req->Body.isValid()) {
        
            // add Content-Length field
            const Ptr<Stream>& bodyStream = req->Body;
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
            const void* responseBytes = [responseData bytes];
            const int responseLength = (const int) [responseData length];
            if (responseLength > 0) {
                Ptr<MemoryStream> responseBody = MemoryStream::Create();
                responseBody->SetURL(req->Url);
                responseBody->Open(OpenMode::WriteOnly);
                responseBody->Reserve(responseLength);
                responseBody->Write(responseBytes, responseLength);
                responseBody->Close();
                if (responseContentType.IsValid()) {
                    responseBody->SetContentType(responseContentType);
                }
                response->Body = responseBody;
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
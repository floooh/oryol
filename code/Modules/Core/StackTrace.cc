//------------------------------------------------------------------------------
//  StackTrace.cc
//------------------------------------------------------------------------------
#if ORYOL_WINDOWS || ORYOL_EMSCRIPTEN || ORYOL_ANDROID
#define HAVE_BACKTRACE (0)
#else
#define HAVE_BACKTRACE (1)
#endif
#include "Pre.h"
#include "StackTrace.h"
#if HAVE_BACKTRACE
#include <execinfo.h>
#endif
#include <cstdlib>
#include <cstring>
#include <cstdio>

/*
    NOTE: The code in StackTrace might be called from signal handlers,
    from any thread, at any time. Be very careful what other functions
    you call!
*/

namespace Oryol {

//------------------------------------------------------------------------------
#if HAVE_BACKTRACE
static char*
appendString(char* str, char* dst, const char* dstEndPtr) {
    if (dst < (dstEndPtr-1)) {
        char c;
        while ((c = *str++) && (dst < (dstEndPtr-1))) {
            *dst++ = c;
        }
        if (dst == (dstEndPtr-1)) {
            --dst;
        }
    }
    // append newline if still room for it
    if (dst < (dstEndPtr-1)) {
        *dst++ = '\n';
    }
    // always terminate with 0
    *dst = 0;
    return dst;
}
#endif

//------------------------------------------------------------------------------
#if HAVE_BACKTRACE
void
StackTrace::Dump(char* buf, int bufSize) {
    buf[0] = 0;
    static const int maxFrames = 64;
    void* frames[maxFrames];
    unsigned int numFrames = backtrace(frames, maxFrames);
    if (0 == numFrames) {
        return;
    }
    char** symbols = backtrace_symbols(frames, numFrames);
    char* dstPtr = buf;
    const char* dstEndPtr = buf + bufSize;
    for (unsigned int i = 0; i < numFrames; i++) {
        dstPtr = appendString(symbols[i], dstPtr, dstEndPtr);
    }
    std::free(symbols);
}

//------------------------------------------------------------------------------
#else
void
StackTrace::Dump(char* buf, int bufSize) {
    std::strncpy(buf, "STACK TRACE NOT IMPLEMENTED\n", bufSize);
    buf[bufSize-1] = 0;
}
#endif

} // namespace Oryol

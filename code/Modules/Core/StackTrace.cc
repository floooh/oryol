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
#include <stdlib.h>
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
    if (dst < (dstEndPtr-2)) {
        char c;
        while ((c = *str++) && (dst < (dstEndPtr-1))) {
            *dst++ = c;
        }
        if (dst == (dstEndPtr-1)) {
            --dst;
        }
    }
    *dst++ = '\n';
    *dst = 0;
    return dst;
}
#endif

//------------------------------------------------------------------------------
#if HAVE_BACKTRACE
void
StackTrace::Dump(char* buf, int bufSize) {
    static const int maxFrames = 64;
    void* frames[maxFrames];
    unsigned int numFrames = backtrace(frames, maxFrames);
    if (0 == numFrames) {
        buf[0] = 0;
        return;
    }
    char** symbols = backtrace_symbols(frames, numFrames);
    char* dstPtr = buf;
    const char* dstEndPtr = buf + bufSize;
    for (unsigned int i = 0; i < numFrames; i++) {
        dstPtr = appendString(symbols[i], dstPtr, dstEndPtr);
    }
    free(symbols);
}

//------------------------------------------------------------------------------
#else
void
StackTrace::Dump(char* buf, int bufSize) {
    // FIXME!
    buf[0] = 0;
}
#endif

} // namespace Oryol

//------------------------------------------------------------------------------
//  StackTrace.cc
//------------------------------------------------------------------------------
#if ORYOL_WINDOWS
#define HAVE_BACKTRACE (0)
#define HAVE_STACKWALKER (1)
#elif ORYOL_EMSCRIPTEN||ORYOL_ANDROID
#define HAVE_BACKTRACE (0)
#define HAVE_STACKWALKER (0)
#else
#define HAVE_BACKTRACE (1)
#define HAVE_STACKWALKER (0)
#endif
#include "Pre.h"
#include "StackTrace.h"
#if HAVE_BACKTRACE
#include <execinfo.h>
#endif
#if HAVE_STACKWALKER
#include "Core/private/windows/StackWalker.h"
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
#if HAVE_BACKTRACE || HAVE_STACKWALKER
static char*
appendString(char* str, char* dst, const char* dstEndPtr, bool insertNewLine) {
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
    if (insertNewLine) {
        if (dst < (dstEndPtr-1)) {
            *dst++ = '\n';
        }
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
        dstPtr = appendString(symbols[i], dstPtr, dstEndPtr, true);
    }
    std::free(symbols);
}

//------------------------------------------------------------------------------
#elif HAVE_STACKWALKER
class OryolStackWalker : public StackWalker {
public:
    // this stuff must be initialized before calling StackWalker::ShowCallstack()
    char* dstPtr = nullptr;
    const char* dstEndPtr = nullptr;

    // constructor to hand options up to parent class
    OryolStackWalker(int options) : StackWalker(options) { };
    // this is called by parent class to output text (originally it calls OutputDebugString)
    virtual void OnOutput(LPCSTR szText) {
        this->dstPtr = appendString((char*)szText, this->dstPtr, this->dstEndPtr, false);
    };
};

void
StackTrace::Dump(char* buf, int bufSize) {
    OryolStackWalker stackWalker(StackWalker::RetrieveSymbol);
    stackWalker.dstPtr = buf;
    stackWalker.dstEndPtr = buf + bufSize;
    stackWalker.ShowCallstack();
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

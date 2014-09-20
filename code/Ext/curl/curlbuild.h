// ORYOL FIX to select between 32-bit and 64-bit curl builds
#if defined __x86_64__
#include "curlbuild-64.h"
#else
#include "curlbuild-32.h"
#endif

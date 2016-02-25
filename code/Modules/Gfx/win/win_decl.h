#pragma once
//------------------------------------------------------------------------------
/** 
    @file win_decl.h
    @brief Windows forward declarations
*/
#if defined(_WIN64)
typedef __int64 INT_PTR, *PINT_PTR;
typedef unsigned __int64 UINT_PTR, *PUINT_PTR;
typedef __int64 LONG_PTR, *PLONG_PTR;
typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
#define __int3264 __int64
#else
typedef int INT_PTR, *PINT_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
#define __int3264 __int32
#endif

typedef void* HANDLE;
typedef unsigned long DWORD;
struct HWND__;
typedef HWND__* HWND;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;
typedef ULONG_PTR SIZE_T, *PSIZE_T;
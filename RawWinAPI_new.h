#ifndef RAWWINAPI_H
#define RAWWINAPI_H

#include <guiddef.h> // for REFIID and other GUID related types
#include <intrin.h>  // for _InterlockedIncrement and _InterlockedDecrement
#include <iostream>  // for std::cerr

// Manually defined GUID and IID structures and related definitions
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;
#endif // GUID_DEFINED

#ifndef COM_REQUIRE_SUCCESS
#define COM_REQUIRE_SUCCESS(hr, comFuncPtrs, msg, ...) \
    if (FAILED(hr)) { \
        std::cerr << msg << std::endl; \
        if (comFuncPtrs.pCoUninitialize) { \
            comFuncPtrs.pCoUninitialize(); \
        } \
        FreeCOMFunctionPointers(comFuncPtrs); \
        __VA_ARGS__; \
    }
#endif // COM_REQUIRE_SUCCESS

typedef GUID IID;
typedef GUID CLSID;
#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
typedef const GUID& REFGUID;
typedef REFGUID REFCLSID;
#endif // _REFGUID_DEFINED

// Windows API types
typedef signed long LONG;
typedef unsigned long ULONG;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef const char* LPCSTR;
typedef int BOOL;
typedef void* HWND;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef void* FARPROC;
typedef void* LPUNKNOWN; // Added definition for LPUNKNOWN

// HRESULT definitions
#ifndef HRESULT
#define HRESULT long
#endif // HRESULT

#ifndef S_OK
#define S_OK ((HRESULT)0L)
#endif // S_OK

// Define _HRESULT_TYPEDEF_ if not already defined
#ifndef _HRESULT_TYPEDEF_
#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
#endif // _HRESULT_TYPEDEF_

#ifndef E_NOINTERFACE
#define E_NOINTERFACE _HRESULT_TYPEDEF_(0x80004002L)
#endif // E_NOINTERFACE

#ifndef FAILED
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif // FAILED

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif // SUCCEEDED

#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE __stdcall
#endif // STDMETHODCALLTYPE

#ifndef WINAPI
#define WINAPI __stdcall
#endif // WINAPI

#define CLSCTX_INPROC_SERVER 0x1

// Interlocked functions
LONG InterlockedIncrement(LONG volatile *Addend);
LONG InterlockedDecrement(LONG volatile *Addend);

// Function pointer types for dynamically loaded functions
typedef HMODULE (WINAPI *PFN_LoadLibraryW)(LPCWSTR lpLibFileName);
typedef BOOL (WINAPI *PFN_FreeLibrary)(HMODULE hLibModule);
typedef FARPROC (WINAPI *PFN_GetProcAddress)(HMODULE hModule, LPCSTR lpProcName);

// Function prototypes
HMODULE LoadLibraryW(LPCWSTR lpLibFileName);
BOOL FreeLibrary(HMODULE hLibModule);
FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

// Helper functions for finding exported functions
HMODULE findModuleBase(void *ptr);
void *findExportedFunction(HMODULE mod, const char *funcName);

// Define REFIID, HRESULT, and other necessary types
#ifndef _REFIID_DEFINED
#define _REFIID_DEFINED
typedef GUID REFIID;
#endif // _REFIID_DEFINED

// IUnknown interface definition
class IUnknown {
private:
    ULONG refCount;
public:
    IUnknown();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
};

// Define IID_IUnknown
#ifndef _IID_IUnknown_
static const IID IID_IUnknown = {0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
#endif // _IID_IUnknown_

#endif // RAWWINAPI_H
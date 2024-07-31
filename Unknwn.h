#ifndef MY_IUNKNOWN_H
#define MY_IUNKNOWN_H

#include <guiddef.h> // For REFIID and GUID

// Define REFIID and GUID if not already defined
#ifndef __IID_DEFINED__
#define __IID_DEFINED__
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;
typedef const GUID *REFIID;
#endif // __IID_DEFINED__

#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
#define REFGUID const GUID &
#endif

// Define HRESULT if not already defined
#ifndef HRESULT_DEFINED
#define HRESULT_DEFINED
typedef long HRESULT;
#endif // HRESULT_DEFINED

// Define ULONG and LONG if not already defined
#ifndef ULONG_DEFINED
#define ULONG_DEFINED
typedef unsigned long ULONG;
#endif // ULONG_DEFINED

#ifndef LONG_DEFINED
#define LONG_DEFINED
typedef long LONG;
#endif // LONG_DEFINED

// Define basic Windows types
typedef void* LPVOID;
typedef void* HWND;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef unsigned int UINT;
typedef int BOOL;
typedef unsigned long DWORD;
typedef void* HMODULE;

// Define LPUNKNOWN
typedef struct IUnknown* LPUNKNOWN;

// Define HRESULT values
#ifndef S_OK
#define S_OK ((HRESULT)0L)
#endif

#ifndef E_NOINTERFACE
#define E_NOINTERFACE ((HRESULT)0x80004002L)
#endif

// Define CLSCTX_INPROC_SERVER
#ifndef CLSCTX_INPROC_SERVER
#define CLSCTX_INPROC_SERVER 0x1
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

// Define FAILED macro
#ifndef FAILED
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif

// Forward declaration for InterlockedIncrement and InterlockedDecrement
extern "C" {
    __declspec(dllimport) LONG __stdcall InterlockedIncrement(LONG volatile *Addend);
    __declspec(dllimport) LONG __stdcall InterlockedDecrement(LONG volatile *Addend);
}

// Define interface macros if not already defined
#ifndef __RPC_FAR
#define __RPC_FAR
#endif

#ifndef _COM_Outptr_
#define _COM_Outptr_
#endif

#ifndef __RPC__deref_out
#define __RPC__deref_out
#endif

#ifndef __RPC__deref_out_opt
#define __RPC__deref_out_opt
#endif

// Define STDMETHODCALLTYPE if not already defined
#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE __stdcall
#endif

// Define STDMETHOD and STDMETHOD_ if not already defined
#ifndef STDMETHOD
#define STDMETHOD(method) virtual HRESULT STDMETHODCALLTYPE method
#endif

#ifndef STDMETHOD_
#define STDMETHOD_(type, method) virtual type STDMETHODCALLTYPE method
#endif

// Define interface macro
#ifndef __interface
#define __interface struct
#endif

// Define BEGIN_INTERFACE and END_INTERFACE if not already defined
#ifndef BEGIN_INTERFACE
#define BEGIN_INTERFACE
#endif

#ifndef END_INTERFACE
#define END_INTERFACE
#endif

// Define the IUnknown interface
__interface IUnknown {
    BEGIN_INTERFACE
    STDMETHOD(QueryInterface)(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject) = 0;

    STDMETHOD_(ULONG, AddRef)(void) = 0;
    STDMETHOD_(ULONG, Release)(void) = 0;
    END_INTERFACE
};

// Define the IID_IUnknown GUID
EXTERN_C const IID IID_IUnknown;

#endif // MY_IUNKNOWN_H

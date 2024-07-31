#ifndef WINUTILS_H
#define WINUTILS_H

#include <string>
#include <unknwn.h>  // For IUnknown

// Function pointer types for the dynamically loaded functions
typedef HRESULT (STDMETHODCALLTYPE *PFN_CoInitialize)(LPVOID);
typedef void (STDMETHODCALLTYPE *PFN_CoUninitialize)();
typedef HRESULT (STDMETHODCALLTYPE *PFN_CoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
typedef HRESULT (STDMETHODCALLTYPE *PFN_SHCreateItemFromParsingName)(LPCWSTR, LPVOID, REFIID, void**);
typedef void (STDMETHODCALLTYPE *PFN_CoTaskMemFree)(LPVOID);

// Structure to hold the function pointers and module handles
struct COMFunctionPointers {
    HMODULE hOle32;
    HMODULE hShell32;
    PFN_CoCreateInstance pCoCreateInstance;
    PFN_CoUninitialize pCoUninitialize;
    PFN_CoTaskMemFree pCoTaskMemFree;
    PFN_CoInitialize pCoInitialize;
    PFN_SHCreateItemFromParsingName pSHCreateItemFromParsingName;
};

// Raw Windows definitions and functions
LPCWSTR string_to_LPCWSTR(const std::wstring& s);
std::wstring LPCWSTR_to_string(LPCWSTR s);

// Function declarations for COM handling
COMFunctionPointers LoadCOMFunctionPointers();
void FreeCOMFunctionPointers(COMFunctionPointers& comFuncPtrs);

#endif // WINUTILS_H

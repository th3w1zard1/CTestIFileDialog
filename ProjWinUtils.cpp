#include "ProjWinUtils.h"

// Helper to convert std::wstring to LPCWSTR
LPCWSTR string_to_LPCWSTR(const std::wstring& s) {
    return s.c_str();
}

// Helper to convert LPCWSTR to std::wstring
std::wstring LPCWSTR_to_string(LPCWSTR s) {
    return std::wstring(s);
}

// Load COM function pointers
COMFunctionPointers LoadCOMFunctionPointers() {
    COMFunctionPointers comFuncPtrs = {0};

    comFuncPtrs.hOle32 = LoadLibraryW(L"ole32.dll");
    comFuncPtrs.hShell32 = LoadLibraryW(L"shell32.dll");

    if (comFuncPtrs.hOle32) {
        comFuncPtrs.pCoCreateInstance = (PFN_CoCreateInstance)GetProcAddress(comFuncPtrs.hOle32, "CoCreateInstance");
        comFuncPtrs.pCoUninitialize = (PFN_CoUninitialize)GetProcAddress(comFuncPtrs.hOle32, "CoUninitialize");
        comFuncPtrs.pCoTaskMemFree = (PFN_CoTaskMemFree)GetProcAddress(comFuncPtrs.hOle32, "CoTaskMemFree");
        comFuncPtrs.pCoInitialize = (PFN_CoInitialize)GetProcAddress(comFuncPtrs.hOle32, "CoInitialize");
    }

    if (comFuncPtrs.hShell32) {
        comFuncPtrs.pSHCreateItemFromParsingName = (PFN_SHCreateItemFromParsingName)GetProcAddress(comFuncPtrs.hShell32, "SHCreateItemFromParsingName");
    }

    return comFuncPtrs;
}

// Free COM libraries
void FreeCOMFunctionPointers(COMFunctionPointers& comFuncPtrs) {
    if (comFuncPtrs.hOle32) {
        FreeLibrary(comFuncPtrs.hOle32);
    }
    if (comFuncPtrs.hShell32) {
        FreeLibrary(comFuncPtrs.hShell32);
    }
}

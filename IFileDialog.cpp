#include <iostream>
#include <vector>
#include <string>
#include <cstdint>  // For fixed-width integer types
#include <limits>
#include <windows.h>
#undef max // Undefine the max macro to prevent limits vs windows.h conflicts
#include <sstream>  // Include the <sstream> header for std::wstringstream
#include "IFileDialog.h"


// Helper to convert std::wstring to LPCWSTR
LPCWSTR string_to_LPCWSTR(const std::wstring& s) {
    return s.c_str();
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
}

void createFileDialog(COMFunctionPointers& comFuncs, IFileDialog** ppFileDialog, bool isSaveDialog) {
    HRESULT hr;
    if (isSaveDialog) {
        hr = comFuncs.pCoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileSaveDialog, reinterpret_cast<void**>(ppFileDialog));
    } else {
        hr = comFuncs.pCoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, reinterpret_cast<void**>(ppFileDialog));
    }
    COM_REQUIRE_SUCCESS(hr, comFuncs, L"Failed to create file dialog", return);
}

void showDialog(COMFunctionPointers& comFuncs, IFileDialog* pFileOpenDialog, HWND hwndOwner) {
    HRESULT hr = pFileOpenDialog->Show(hwndOwner);
    COM_REQUIRE_SUCCESS(hr, comFuncs, "Failed to show the file open dialog", return);
}

std::vector<std::wstring> getFileDialogResults(COMFunctionPointers& comFuncs, IFileOpenDialog* pFileOpenDialog) {
    std::vector<std::wstring> results;
    IShellItemArray* pResultsArray;
    HRESULT hr = pFileOpenDialog->GetResults(&pResultsArray);
    if (FAILED(hr)) {
        std::cerr << "Failed to get dialog results" << std::endl;
        comFuncs.pCoUninitialize();
        return results; // Return the default constructed results vector
    }

    DWORD itemCount;
    hr = pResultsArray->GetCount(&itemCount);
    if (FAILED(hr)) {
        std::cerr << "Failed to get item count" << std::endl;
        pResultsArray->Release();
        comFuncs.pCoUninitialize();
        return results; // Return the default constructed results vector
    }

    for (DWORD i = 0; i < itemCount; ++i) {
        IShellItem* pItem;
        hr = pResultsArray->GetItemAt(i, reinterpret_cast<IUnknown**>(&pItem));
        if (FAILED(hr)) {
            std::cerr << "Failed to get item" << std::endl;
            continue; // Continue to the next item
        }

        LPWSTR pszFilePath;
        hr = pItem->GetDisplayName(0, &pszFilePath);
        if (FAILED(hr)) {
            std::cerr << "Failed to get file path" << std::endl;
            pItem->Release();
            continue; // Continue to the next item
        }

        results.push_back(std::wstring(pszFilePath));
        comFuncs.pCoTaskMemFree(pszFilePath);
        pItem->Release();
    }

    pResultsArray->Release();
    return results;
}

// Configure the file dialog
void configureFileDialog(COMFunctionPointers& comFuncs, IFileDialog* pFileDialog, const std::vector<COMDLG_FILTERSPEC>& filters, const std::wstring& defaultFolder, DWORD options) {
    if (!filters.empty()) {
        HRESULT hr = pFileDialog->SetFileTypes(static_cast<UINT>(filters.size()), filters.data());
        COM_REQUIRE_SUCCESS(hr, comFuncs, L"Failed to set file types", return);
    }

    if (!defaultFolder.empty()) {
        IShellItem* pFolder;
        HRESULT hr = comFuncs.pSHCreateItemFromParsingName(defaultFolder.c_str(), NULL, IID_IShellItem, reinterpret_cast<void**>(&pFolder));
        COM_REQUIRE_SUCCESS(hr, comFuncs, L"Failed to create shell item from default folder", return);
        hr = pFileDialog->SetFolder(pFolder);
        COM_REQUIRE_SUCCESS(hr, comFuncs, L"Failed to set default folder", return);
        pFolder->Release();
    }

    HRESULT hr = pFileDialog->SetOptions(options);
    COM_REQUIRE_SUCCESS(hr, comFuncs, L"Failed to set dialog options", return);
}

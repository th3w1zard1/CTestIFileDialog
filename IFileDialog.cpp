#include <iostream>
#include <vector>
#include <string>
#include <cstdint>  // For fixed-width integer types
#include <limits>
//#include <shlobj.h>  // defines the interfaces we already have defined here, uncomment for testing purposes
//#include <shlwapi.h>
#include <sstream>  // Include the <sstream> header for std::wstringstream
#include "IFileDialog.h"


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
}

class FileDialogEventHandler : public IFileDialogEvents {
public:
    FileDialogEventHandler() : refCount(1) {}

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv) {
        if (riid == IID_IUnknown || riid == IID_IFileDialogEvents) {
            *ppv = static_cast<IFileDialogEvents*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        return InterlockedIncrement(&refCount);
    }

    ULONG STDMETHODCALLTYPE Release() {
        ULONG count = InterlockedDecrement(&refCount);
        if (count == 0) {
            delete this;
        }
        return count;
    }

    // IFileDialogEvents methods
    HRESULT STDMETHODCALLTYPE OnFileOk(IFileDialog *pfd) { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnFolderChanging(IFileDialog *pfd, IShellItem *psiFolder) { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnFolderChange(IFileDialog *pfd) { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnSelectionChange(IFileDialog *pfd) { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnShareViolation(IFileDialog *pfd, IShellItem *psi, FDE_SHAREVIOLATION_RESPONSE *pResponse) { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnTypeChange(IFileDialog *pfd) { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnOverwrite(IFileDialog *pfd, IShellItem *psi, FDE_OVERWRITE_RESPONSE *pResponse) { return S_OK; }

protected:
    virtual ~FileDialogEventHandler() = default;

private:
    LONG refCount;
};

void createFileDialog(COMFunctionPointers& comFuncs, IFileDialog** ppFileDialog, bool isSaveDialog) {
    HRESULT hr;
    if (isSaveDialog) {
        hr = comFuncs.pCoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileSaveDialog, reinterpret_cast<void**>(ppFileDialog));
    } else {
        hr = comFuncs.pCoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, reinterpret_cast<void**>(ppFileDialog));
    }
    COM_REQUIRE_SUCCESS(hr, comFuncs, L"Failed to create file dialog", return);
}

void showDialog(COMFunctionPointers& comFuncs, IFileDialog* pFileOpenDialog, HWND hwndOwner) {
    HRESULT hr = pFileOpenDialog->Show(hwndOwner);
    COM_REQUIRE_SUCCESS(hr, comFuncs, L"Failed to show the file open dialog", return);
}

std::vector<std::wstring> getFileDialogResults(COMFunctionPointers& comFuncs, IFileOpenDialog* pFileOpenDialog) {
    std::vector<std::wstring> results;
    IShellItemArray* pResultsArray;
    HRESULT hr = pFileOpenDialog->GetResults(&pResultsArray);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to get dialog results" << std::endl;
        comFuncs.pCoUninitialize();
        return results; // Return the default constructed results vector
    }

    DWORD itemCount;
    hr = pResultsArray->GetCount(&itemCount);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to get item count" << std::endl;
        pResultsArray->Release();
        comFuncs.pCoUninitialize();
        return results; // Return the default constructed results vector
    }

    for (DWORD i = 0; i < itemCount; ++i) {
        IShellItem* pItem;
        //hr = pResultsArray->GetItemAt(i, reinterpret_cast<IUnknown**>(&pItem));
        hr = pResultsArray->GetItemAt(i, &pItem);
        if (FAILED(hr)) {
            std::wcerr << L"Failed to get item" << std::endl;
            continue; // Continue to the next item
        }

        LPWSTR pszFilePath;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        if (FAILED(hr)) {
            std::wcerr << L"Failed to get file path" << std::endl;
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

/*void eatMouseMove() {
    MSG msg = {0, 0, 0, 0, 0, {0, 0} };
    while (PeekMessage(&msg, 0, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
    if (msg.message == WM_MOUSEMOVE)
        PostMessage(msg.hwnd, msg.message, 0, msg.lParam);
    std::wcout << L"eatMouseMove triggered=" << (msg.message == WM_MOUSEMOVE) << std::endl;
}*/

// Helper function to create a shell item from a path
IShellItem* createShellItem(COMFunctionPointers& comFuncs, const std::wstring& path) {
    IShellItem* pItem = nullptr;
    HRESULT hr = comFuncs.pSHCreateItemFromParsingName(path.c_str(), NULL, IID_IShellItem, reinterpret_cast<void**>(&pItem));
    if (FAILED(hr)) {
        std::wcerr << L"Failed to create shell item from path: " << path << std::endl;
    }
    return pItem;
}

// Helper function to get file paths from IShellItemArray
std::vector<std::wstring> getFilePathsFromShellItemArray(IShellItemArray* pItemArray) {
    std::vector<std::wstring> filePaths;
    DWORD itemCount = 0;
    HRESULT hr = pItemArray->GetCount(&itemCount);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to get item count from shell item array" << std::endl;
        return filePaths;
    }

    COMFunctionPointers comFuncs = LoadCOMFunctionPointers();

    for (DWORD i = 0; i < itemCount; ++i) {
        IShellItem* pItem = nullptr;
        //hr = pItemArray->GetItemAt(i, reinterpret_cast<IUnknown**>(&pItem));
        hr = pItemArray->GetItemAt(i, &pItem);
        if (FAILED(hr)) {
            std::wcerr << L"Failed to get item from shell item array" << std::endl;
            continue;
        }

        LPWSTR pszFilePath = nullptr;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        if (FAILED(hr)) {
            std::wcerr << L"Failed to get display name from shell item" << std::endl;
            pItem->Release();
            continue;
        }

        filePaths.push_back(std::wstring(pszFilePath));
        comFuncs.pCoTaskMemFree(pszFilePath);
        pItem->Release();
    }

    return filePaths;
}

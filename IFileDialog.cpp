#include "IFileDialog.h"
#include "ProjWinUtils.h"
#include <iostream>
#include <vector>
#include <string>

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
    IShellItemArray* pResultsArray = nullptr;
    HRESULT hr = pFileOpenDialog->GetResults(&pResultsArray);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to get dialog results. HRESULT: " << hr << std::endl;
        comFuncs.pCoUninitialize();
        return results;
    }

    DWORD itemCount = 0;
    hr = pResultsArray->GetCount(&itemCount);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to get item count. HRESULT: " << hr << std::endl;
        pResultsArray->Release();
        comFuncs.pCoUninitialize();
        return results;
    }

    std::wcout << L"Number of items selected: " << itemCount << std::endl;

    IEnumShellItems* pEnumShellItems = nullptr;
    hr = pResultsArray->EnumItems(&pEnumShellItems);
    if (SUCCEEDED(hr) && pEnumShellItems) {
        IShellItem* pEnumItem = nullptr;
        ULONG fetched = 0;
        while (pEnumShellItems->Next(1, &pEnumItem, &fetched) == S_OK && fetched > 0) {
            std::wcout << L"Enumerating item: " << std::endl;
            LPWSTR pszName = nullptr;
            hr = pEnumItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pszName);
            if (SUCCEEDED(hr)) {
                std::wcout << L" - Display name: " << pszName << std::endl;
                comFuncs.pCoTaskMemFree(pszName);
            }

            LPWSTR pszFilePath = nullptr;
            hr = pEnumItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
            if (SUCCEEDED(hr)) {
                std::wcout << L" - File path: " << pszFilePath << std::endl;
                comFuncs.pCoTaskMemFree(pszFilePath);
            }

            SFGAOF attributes;
            hr = pEnumItem->GetAttributes(SFGAO_FILESYSTEM | SFGAO_FOLDER, &attributes);
            if (SUCCEEDED(hr)) {
                std::wcout << L" - Attributes: " << attributes << std::endl;
            }

            IShellItem* pParentItem = nullptr;
            hr = pEnumItem->GetParent(&pParentItem);
            if (SUCCEEDED(hr) && pParentItem) {
                LPWSTR pszParentName = nullptr;
                hr = pParentItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pszParentName);
                if (SUCCEEDED(hr)) {
                    std::wcout << L" - Parent: " << pszParentName << std::endl;
                    comFuncs.pCoTaskMemFree(pszParentName);
                }
                pParentItem->Release();
            }

            pEnumItem->Release();
        }
        pEnumShellItems->Release();
    }

    for (DWORD i = 0; i < itemCount; ++i) {
        IShellItem* pItem = nullptr;
        hr = pResultsArray->GetItemAt(i, &pItem);
        if (FAILED(hr)) {
            std::wcerr << L"Failed to get item at index " << i << L". HRESULT: " << hr << std::endl;
            continue;
        }

        LPWSTR pszFilePath = nullptr;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        if (SUCCEEDED(hr)) {
            results.push_back(std::wstring(pszFilePath));
            std::wcout << L"Item " << i << L" file path: " << pszFilePath << std::endl;
            comFuncs.pCoTaskMemFree(pszFilePath);
        } else {
            std::wcerr << L"Failed to get file path for item " << i << L". HRESULT: " << hr << std::endl;
        }

        SFGAOF sfgaoAttribs = 0;
        hr = pItem->GetAttributes(SFGAO_FILESYSTEM, &sfgaoAttribs);
        if (SUCCEEDED(hr)) {
            std::wcout << L"Item " << i << L" attributes: " << sfgaoAttribs << std::endl;
        } else {
            std::wcerr << L"Failed to get attributes for item " << i << L". HRESULT: " << hr << std::endl;
        }

        IShellItem* pParentItem = nullptr;
        hr = pItem->GetParent(&pParentItem);
        if (SUCCEEDED(hr) && pParentItem) {
            LPWSTR pszParentName = nullptr;
            hr = pParentItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pszParentName);
            if (SUCCEEDED(hr)) {
                std::wcout << L"Item " << i << L" parent: " << pszParentName << std::endl;
                comFuncs.pCoTaskMemFree(pszParentName);
            } else {
                std::wcerr << L"Failed to get parent display name for item " << i << L". HRESULT: " << hr << std::endl;
            }
            pParentItem->Release();
        } else {
            std::wcerr << L"Failed to get parent item for item " << i << L". HRESULT: " << hr << std::endl;
        }

        int comparisonResult;
        hr = pItem->Compare(pItem, SICHINT_CANONICAL, &comparisonResult);
        if (SUCCEEDED(hr)) {
            std::wcout << L"Item " << i << L" comparison result (self): " << comparisonResult << std::endl;
        } else {
            std::wcerr << L"Failed to compare item " << i << L" with itself. HRESULT: " << hr << std::endl;
        }

        pItem->Release();
    }

    pResultsArray->Release();
    return results;
}

void setDialogAttributes(IFileDialog* pFileDialog, const std::wstring& title, const std::wstring& okButtonLabel, const std::wstring& fileNameLabel) {
    if (!title.empty()) {
        pFileDialog->SetTitle(title.c_str());
    }

    if (!okButtonLabel.empty()) {
        pFileDialog->SetOkButtonLabel(okButtonLabel.c_str());
    }

    if (!fileNameLabel.empty()) {
        pFileDialog->SetFileName(fileNameLabel.c_str());
    }
}

// Configure the file dialog
void configureFileDialog(COMFunctionPointers& comFuncs, IFileDialog* pFileDialog, const std::vector<COMDLG_FILTERSPEC>& filters, const std::wstring& defaultFolder, DWORD options, bool forceFileSystem, bool allowMultiselect) {
    if (!filters.empty()) {
        HRESULT hr = pFileDialog->SetFileTypes(static_cast<UINT>(filters.size()), filters.data());
        COM_REQUIRE_SUCCESS(hr, comFuncs, L"Failed to set file types", return);
    }

    if (!defaultFolder.empty()) {
        IShellItem* pFolder = createShellItem(comFuncs, defaultFolder);
        if (pFolder) {
            HRESULT hr = pFileDialog->SetFolder(pFolder);
            COM_REQUIRE_SUCCESS(hr, comFuncs, L"Failed to set default folder", return);
            pFolder->Release();
        }
    }

    if (forceFileSystem) {
        options |= FOS_FORCEFILESYSTEM;
    }

    if (allowMultiselect) {
        options |= FOS_ALLOWMULTISELECT;
    }

    HRESULT hr = pFileDialog->SetOptions(options);
    COM_REQUIRE_SUCCESS(hr, comFuncs, L"Failed to set dialog options", return);
}


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
std::vector<std::wstring> getFilePathsFromShellItemArray(IShellItemArray* pItemArray, COMFunctionPointers& comFuncs) {
    std::vector<std::wstring> filePaths;
    DWORD itemCount = 0;
    HRESULT hr = pItemArray->GetCount(&itemCount);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to get item count from shell item array" << std::endl;
        return filePaths;
    }

    for (DWORD i = 0; i < itemCount; ++i) {
        IShellItem* pItem = nullptr;
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

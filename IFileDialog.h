#ifndef IFILEDIALOG_H
#define IFILEDIALOG_H

#include <vector>
#include <string>
#include <cstdint>  // For fixed-width integer types
#include <windows.h>
#undef max // Undefine the max macro to prevent conflicts


// Manually defined GUID and IID structures and related definitions
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;
#endif

typedef GUID IID;
typedef GUID CLSID;
#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
typedef const GUID& REFGUID;
typedef REFGUID REFCLSID;
#endif

// Windows API types
typedef unsigned long ULONG;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
#ifndef _WINDOWS_
typedef void* HWND;
typedef GUID REFIID;
typedef void* HMODULE;
#endif
typedef void* LPVOID;

// Windows API macros
#define STDMETHODCALLTYPE __stdcall

// HRESULT definitions
#ifndef HRESULT
#define HRESULT long
#endif

#ifndef S_OK
#define S_OK ((HRESULT)0L)
#endif

#ifndef FAILED
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif
//#include "RawWinAPI.h"
// REM: might end up needing these
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#include <minwindef.h>


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
#endif

// Function pointer types for the dynamically loaded functions
typedef HRESULT (STDMETHODCALLTYPE *PFN_CoInitialize)(LPVOID);
typedef void (STDMETHODCALLTYPE *PFN_CoUninitialize)();
typedef HRESULT (STDMETHODCALLTYPE *PFN_CoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
typedef HRESULT (STDMETHODCALLTYPE *PFN_SHCreateItemFromParsingName)(LPCWSTR, LPVOID, REFIID, void**);
typedef void (STDMETHODCALLTYPE *PFN_CoTaskMemFree)(LPVOID);

// CLSCTX_INPROC_SERVER definition
#define CLSCTX_INPROC_SERVER 0x1

// COM interface definitions
#ifndef _WINDOWS_
struct IUnknown {
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) = 0;
    virtual ULONG STDMETHODCALLTYPE AddRef() = 0;
    virtual ULONG STDMETHODCALLTYPE Release() = 0;
};
#endif

struct IModalWindow : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Show(HWND hwndOwner) = 0;
};

struct IShellItemArray : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(DWORD *pdwNumItems) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetItemAt(DWORD dwIndex, IUnknown **ppsi) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumItems(IUnknown **ppenumShellItems) = 0;
};

struct IShellItem : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE BindToHandler(IUnknown *pbc, REFGUID bhid, REFIID riid, void **ppv) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetParent(IShellItem **ppsi) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDisplayName(int sigdnName, LPWSTR *ppszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAttributes(ULONG sfgaoMask, ULONG *psfgaoAttribs) = 0;
    virtual HRESULT STDMETHODCALLTYPE Compare(IShellItem *psi, DWORD hint, int *piOrder) = 0;
};

struct IShellItemFilter : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IncludeItem(IShellItem *psi) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEnumFlagsForItem(IShellItem *psi, DWORD *pgrfFlags) = 0;
};

struct IEnumShellItems : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, IShellItem **rgelt, ULONG *pceltFetched) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset() = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumShellItems **ppenum) = 0;
};

struct IFileDialog : public IModalWindow {
    virtual HRESULT STDMETHODCALLTYPE SetFileTypes(UINT cFileTypes, const struct _COMDLG_FILTERSPEC *rgFilterSpec) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFileTypeIndex(UINT iFileType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFileTypeIndex(UINT *piFileType) = 0;
    virtual HRESULT STDMETHODCALLTYPE Advise(IUnknown *pfde, DWORD *pdwCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD dwCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOptions(DWORD fos) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOptions(DWORD *pfos) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultFolder(IShellItem *psi) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFolder(IShellItem *psi) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFolder(IShellItem **ppsi) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSelection(IShellItem **ppsi) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFileName(LPCWSTR pszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFileName(LPWSTR *pszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTitle(LPCWSTR pszTitle) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOkButtonLabel(LPCWSTR pszText) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFileNameLabel(LPCWSTR pszLabel) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResult(IShellItem **ppsi) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddPlace(IShellItem *psi, int fdap) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultExtension(LPCWSTR pszDefaultExtension) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(HRESULT hr) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetClientGuid(REFGUID guid) = 0;
    virtual HRESULT STDMETHODCALLTYPE ClearClientData() = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFilter(IShellItemFilter *pFilter) = 0;
};

struct IFileDialogEvents : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE OnFileOk(IFileDialog *pfd) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnFolderChanging(IFileDialog *pfd, IShellItem *psiFolder) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnFolderChange(IFileDialog *pfd) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnSelectionChange(IFileDialog *pfd) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnShareViolation(IFileDialog *pfd, IShellItem *psi, int *pResponse) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnTypeChange(IFileDialog *pfd) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnOverwrite(IFileDialog *pfd, IShellItem *psi, int *pResponse) = 0;
};

struct IFileOpenDialog : public IFileDialog {
    virtual HRESULT STDMETHODCALLTYPE GetResults(IShellItemArray **ppenum) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSelectedItems(IShellItemArray **ppsai) = 0;
};

struct IFileSaveDialog : public IFileDialog {
    virtual HRESULT STDMETHODCALLTYPE SetSaveAsItem(IShellItem* psi) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProperties(IUnknown* pStore) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCollectedProperties(IUnknown* pStore, BOOL fAppendDefault) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperties(IUnknown** ppStore) = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyProperties(IShellItem* psi, IUnknown* pStore, HWND hwnd, IUnknown* pSink) = 0;
};

// COM constants
const CLSID CLSID_FileOpenDialog = {0xDC1C5A9C, 0xE88A, 0x4DDE, {0xA5, 0xA1, 0x60, 0xF8, 0x2A, 0x20, 0xAE, 0xF7}};
const IID IID_IFileOpenDialog = {0xD57C7288, 0xD4AD, 0x4768, {0xBE, 0x02, 0x9D, 0x96, 0x95, 0x32, 0xD9, 0x60}};
const IID IID_IShellItem = {0x43826D1E, 0xE718, 0x42EE, {0xBC, 0x55, 0xA1, 0xE2, 0x61, 0xC3, 0x7B, 0xFE}};
const IID IID_IFileSaveDialog = {0x84BCCDC3, 0x5FDE, 0x4CDB, {0xAE, 0xA4, 0xAF, 0x64, 0xB8, 0x3D, 0x78, 0xAB}};

// Additional COMDLG_FILTERSPEC
typedef IUnknown* LPUNKNOWN;
typedef struct _COMDLG_FILTERSPEC {
    LPCWSTR pszName;
    LPCWSTR pszSpec;
} COMDLG_FILTERSPEC;

#define FOS_OVERWRITEPROMPT      0x00000002
#define FOS_STRICTFILETYPES      0x00000004
#define FOS_NOCHANGEDIR          0x00000008
#define FOS_PICKFOLDERS          0x00000020
#define FOS_FORCEFILESYSTEM      0x00000040
#define FOS_ALLNONSTORAGEITEMS   0x00000080
#define FOS_NOVALIDATE           0x00000100
#define FOS_ALLOWMULTISELECT     0x00000200
#define FOS_PATHMUSTEXIST        0x00000800
#define FOS_FILEMUSTEXIST        0x00001000
#define FOS_CREATEPROMPT         0x00002000
#define FOS_SHAREAWARE           0x00004000
#define FOS_NOREADONLYRETURN     0x00008000
#define FOS_NOTESTFILECREATE     0x00010000
#define FOS_HIDEMRUPLACES        0x00020000
#define FOS_HIDEPINNEDPLACES     0x00040000
#define FOS_NODEREFERENCELINKS   0x00100000
#define FOS_DONTADDTORECENT      0x02000000
#define FOS_FORCESHOWHIDDEN      0x10000000
#define FOS_DEFAULTNOMINIMODE    0x20000000


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

// Function declarations
COMFunctionPointers LoadCOMFunctionPointers();
void FreeCOMFunctionPointers(COMFunctionPointers& comFuncPtrs);
void createFileDialog(COMFunctionPointers& comFuncs, IFileDialog** ppFileDialog, bool isSaveDialog);
void showDialog(COMFunctionPointers& comFuncs, IFileDialog* pFileOpenDialog, HWND hwndOwner = NULL);
std::vector<std::wstring> getFileDialogResults(COMFunctionPointers& comFuncs, IFileOpenDialog* pFileOpenDialog);
void configureFileDialog(COMFunctionPointers& comFuncs, IFileDialog* pFileDialog, const std::vector<COMDLG_FILTERSPEC>& filters, const std::wstring& defaultFolder, DWORD options);


// Helper to convert std::wstring to LPCWSTR
LPCWSTR string_to_LPCWSTR(const std::wstring& s);
#endif // IFILEDIALOG_H
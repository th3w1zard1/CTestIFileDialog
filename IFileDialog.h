#ifndef IFILEDIALOG_H
#define IFILEDIALOG_H

#include <vector>
#include <string>
#include <cstdint>  // For fixed-width integer types
//#include "RawWinAPI.h"
// REM: might end up needing these
#include <shlobj.h>  // defines the interfaces we already have defined here, uncomment for testing purposes
#include <shlwapi.h>
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#include <minwindef.h>

// COM interface definitions


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

#ifndef __IModalWindow_FWD_DEFINED__
#define __IModalWindow_FWD_DEFINED__
struct IModalWindow : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Show(HWND hwndOwner) = 0;
};
#endif // __IModalWindow_FWD_DEFINED__

#ifndef __IShellItemArray_FWD_DEFINED__
#define __IShellItemArray_FWD_DEFINED__
struct IShellItemArray : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(DWORD *pdwNumItems) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetItemAt(DWORD dwIndex, IUnknown **ppsi) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumItems(IUnknown **ppenumShellItems) = 0;
};
#endif // __IShellItemArray_FWD_DEFINED__

#ifndef __IShellItem_FWD_DEFINED__
#define __IShellItem_FWD_DEFINED__
struct IShellItem : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE BindToHandler(IUnknown *pbc, REFGUID bhid, REFIID riid, void **ppv) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetParent(IShellItem **ppsi) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDisplayName(int sigdnName, LPWSTR *ppszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAttributes(ULONG sfgaoMask, ULONG *psfgaoAttribs) = 0;
    virtual HRESULT STDMETHODCALLTYPE Compare(IShellItem *psi, DWORD hint, int *piOrder) = 0;
};
#endif // __IShellItem_FWD_DEFINED__

#ifndef __IShellItemFilter_FWD_DEFINED__
#define __IShellItemFilter_FWD_DEFINED__
struct IShellItemFilter : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IncludeItem(IShellItem *psi) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEnumFlagsForItem(IShellItem *psi, DWORD *pgrfFlags) = 0;
};
#endif // __IShellItemFilter_FWD_DEFINED__

#ifndef __IEnumShellItems_FWD_DEFINED__
#define __IEnumShellItems_FWD_DEFINED__
struct IEnumShellItems : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, IShellItem **rgelt, ULONG *pceltFetched) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset() = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumShellItems **ppenum) = 0;
};
#endif // __IEnumShellItems_FWD_DEFINED__

#ifndef __IFileDialog_FWD_DEFINED__
#define __IFileDialog_FWD_DEFINED__
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
#endif // __IFileDialog_FWD_DEFINED__

#ifndef __IFileDialogEvents_FWD_DEFINED__
#define __IFileDialogEvents_FWD_DEFINED__
struct IFileDialogEvents : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE OnFileOk(IFileDialog *pfd) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnFolderChanging(IFileDialog *pfd, IShellItem *psiFolder) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnFolderChange(IFileDialog *pfd) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnSelectionChange(IFileDialog *pfd) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnShareViolation(IFileDialog *pfd, IShellItem *psi, int *pResponse) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnTypeChange(IFileDialog *pfd) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnOverwrite(IFileDialog *pfd, IShellItem *psi, int *pResponse) = 0;
};
#endif // __IFileDialogEvents_FWD_DEFINED__

#ifndef __IFileOpenDialog_FWD_DEFINED__
#define __IFileOpenDialog_FWD_DEFINED__
struct IFileOpenDialog : public IFileDialog {
    virtual HRESULT STDMETHODCALLTYPE GetResults(IShellItemArray **ppenum) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSelectedItems(IShellItemArray **ppsai) = 0;
};
#endif // __IFileOpenDialog_FWD_DEFINED__

#ifndef __IFileSaveDialog_FWD_DEFINED__
#define __IFileSaveDialog_FWD_DEFINED__
struct IFileSaveDialog : public IFileDialog {
    virtual HRESULT STDMETHODCALLTYPE SetSaveAsItem(IShellItem* psi) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProperties(IUnknown* pStore) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCollectedProperties(IUnknown* pStore, BOOL fAppendDefault) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperties(IUnknown** ppStore) = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyProperties(IShellItem* psi, IUnknown* pStore, HWND hwnd, IUnknown* pSink) = 0;
};
#endif // __IFileSaveDialog_FWD_DEFINED__

#ifndef __IShellLibrary_FWD_DEFINED__
#define __IShellLibrary_FWD_DEFINED__
// COM constants
static const IID IID_IFileOpenDialog = {0xd57c7288, 0xd4ad, 0x4768, {0xbe, 0x02, 0x9d, 0x96, 0x95, 0x32, 0xd9, 0x60}};
static const IID IID_IFileSaveDialog = {0x84bccd23, 0x5fde, 0x4cdb,{0xae, 0xa4, 0xaf, 0x64, 0xb8, 0x3d, 0x78, 0xab}};
static const IID IID_IShellItem = {0x43826d1e, 0xe718, 0x42ee, {0xbc, 0x55, 0xa1, 0xe2, 0x61, 0xc3, 0x7b, 0xfe}};
static const IID IID_IShellItemArray = {0xb63ea76d, 0x1f85, 0x456f, {0xa1, 0x9c, 0x48, 0x15, 0x9e, 0xfa, 0x85, 0x8b}};
static const IID IID_IFileDialogEvents = {0x973510db, 0x7d7f, 0x452b,{0x89, 0x75, 0x74, 0xa8, 0x58, 0x28, 0xd3, 0x54}};
static const CLSID CLSID_FileOpenDialog = {0xdc1c5a9c, 0xe88a, 0x4dde, {0xa5, 0xa1, 0x60, 0xf8, 0x2a, 0x20, 0xae, 0xf7}};
static const CLSID CLSID_FileSaveDialog = {0xc0b4e2f3, 0xba21, 0x4773,{0x8d, 0xba, 0x33, 0x5e, 0xc9, 0x46, 0xeb, 0x8b}};
#ifndef __REQUIRED_RPCNDR_H_VERSION__
    typedef struct _COMDLG_FILTERSPEC{
            LPCWSTR pszName;
            LPCWSTR pszSpec;
    } COMDLG_FILTERSPEC;

#endif  // NTDDI_VISTA
#define FOS_OVERWRITEPROMPT 0x2
#define FOS_STRICTFILETYPES 0x4
#define FOS_NOCHANGEDIR 0x8
#define FOS_PICKFOLDERS 0x20
#define FOS_FORCEFILESYSTEM 0x40
#define FOS_ALLNONSTORAGEITEMS 0x80
#define FOS_NOVALIDATE 0x100
#define FOS_ALLOWMULTISELECT 0x200
#define FOS_PATHMUSTEXIST 0x800
#define FOS_FILEMUSTEXIST 0x1000
#define FOS_CREATEPROMPT 0x2000
#define FOS_SHAREAWARE 0x4000
#define FOS_NOREADONLYRETURN 0x8000
#define FOS_NOTESTFILECREATE 0x10000
#define FOS_HIDEMRUPLACES 0x20000
#define FOS_HIDEPINNEDPLACES 0x40000
#define FOS_NODEREFERENCELINKS 0x100000
#define FOS_DONTADDTORECENT 0x2000000
#define FOS_FORCESHOWHIDDEN 0x10000000
#define FOS_DEFAULTNOMINIMODE 0x20000000
#define FOS_FORCEPREVIEWPANEON 0x40000000
typedef int GETPROPERTYSTOREFLAGS;
#define GPS_DEFAULT 0x00000000
#define GPS_HANDLERPROPERTIESONLY 0x00000001
#define GPS_READWRITE 0x00000002
#define GPS_TEMPORARY 0x00000004
#define GPS_FASTPROPERTIESONLY 0x00000008
#define GPS_OPENSLOWITEM 0x00000010
#define GPS_DELAYCREATION 0x00000020
#define GPS_BESTEFFORT 0x00000040
#define GPS_MASK_VALID 0x0000007F
#define BFFM_INITIALIZED 1
#define BFFM_SELCHANGED 2
#define BFFM_ENABLEOK (WM_USER + 101)
#define BIF_NONEWFOLDERBUTTON 0x0200
#define BIF_NOTRANSLATETARGETS 0x0400
#define BIF_BROWSEFORCOMPUTER 0x1000
#define BIF_BROWSEFORPRINTER 0x2000
#define BIF_BROWSEINCLUDEFILES 0x4000
#define BIF_SHAREABLE 0x8000
typedef enum {
    SIATTRIBFLAGS_AND = 0x1,
    SIATTRIBFLAGS_OR = 0x2,
    SIATTRIBFLAGS_APPCOMPAT = 0x3,
    SIATTRIBFLAGS_MASK = 0x3
} SIATTRIBFLAGS;
typedef enum {
    SIGDN_NORMALDISPLAY = 0x00000000,
    SIGDN_PARENTRELATIVEPARSING = 0x80018001,
    SIGDN_PARENTRELATIVEFORADDRESSBAR = 0x8001c001,
    SIGDN_DESKTOPABSOLUTEPARSING = 0x80028000,
    SIGDN_PARENTRELATIVEEDITING = 0x80031001,
    SIGDN_DESKTOPABSOLUTEEDITING = 0x8004c000,
    SIGDN_FILESYSPATH = 0x80058000,
    SIGDN_URL = 0x80068000
} SIGDN;
typedef enum {
    FDAP_BOTTOM = 0x00000000,
    FDAP_TOP = 0x00000001
} FDAP;
typedef enum {
    FDESVR_DEFAULT = 0x00000000,
    FDESVR_ACCEPT = 0x00000001,
    FDESVR_REFUSE = 0x00000002
} FDE_SHAREVIOLATION_RESPONSE;
typedef FDE_SHAREVIOLATION_RESPONSE FDE_OVERWRITE_RESPONSE;
#endif // __IShellLibrary_FWD_DEFINED__

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

// Function declarations
COMFunctionPointers LoadCOMFunctionPointers();
void FreeCOMFunctionPointers(COMFunctionPointers& comFuncPtrs);
void createFileDialog(COMFunctionPointers& comFuncs, IFileDialog** ppFileDialog, bool isSaveDialog);
void showDialog(COMFunctionPointers& comFuncs, IFileDialog* pFileOpenDialog, HWND hwndOwner = NULL);
std::vector<std::wstring> getFileDialogResults(COMFunctionPointers& comFuncs, IFileOpenDialog* pFileOpenDialog);
void configureFileDialog(COMFunctionPointers& comFuncs, IFileDialog* pFileDialog, const std::vector<COMDLG_FILTERSPEC>& filters, const std::wstring& defaultFolder, DWORD options);


LPCWSTR string_to_LPCWSTR(const std::wstring& s);  // Helper to convert std::wstring to LPCWSTR
std::wstring LPCWSTR_to_string(LPCWSTR s);  // Helper to convert LPCWSTR to std::wstring
#endif // IFILEDIALOG_H
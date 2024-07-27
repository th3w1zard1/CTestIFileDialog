#ifndef IFILEDIALOG_H
#define IFILEDIALOG_H

#include <vector>
#include <string>
#include <cstdint>  // For fixed-width integer types
//#include <ShObjIdl.h>  // defines the interfaces we already have defined here, uncomment for testing/comparison purposes
//#include "RawWinAPI.h" // experimental alternative to IUnknown's definition and similar.
#include <shlwapi.h>  // For STDMETHODCALLTYPE

// Toggle definitions
//#define TOGGLE_IModalWindow
#define TOGGLE_IShellItemArray
//#define TOGGLE_IShellItem
//#define TOGGLE_IShellItemFilter
//#define TOGGLE_IEnumShellItems
//#define TOGGLE_IFileDialog
//#define TOGGLE_IFileDialogEvents
//#define TOGGLE_IFileOpenDialog
//#define TOGGLE_IFileSaveDialog
#define FORCE_MY_INTERFACE

// Macro to simplify the interface definitions
#define DEFINE_INTERFACE(iface, base, methods) \
    struct iface : public base { \
        methods \
    };

#define DEFINE_MY_INTERFACE(iface, myiface, base, methods) \
    struct myiface : public base { \
        methods \
    };

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

// Macro to define methods
#define DEFINE_IShellItemArray_METHODS \
    virtual HRESULT STDMETHODCALLTYPE GetCount(DWORD *pdwNumItems) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetItemAt(DWORD dwIndex, MyIShellItem **ppsi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE EnumItems(IUnknown **ppenumShellItems) = 0;

#define DEFINE_IShellItem_METHODS \
    virtual HRESULT STDMETHODCALLTYPE BindToHandler(IUnknown *pbc, REFGUID bhid, REFIID riid, void **ppv) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetParent(MyIShellItem **ppsi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetDisplayName(int sigdnName, LPWSTR *ppszName) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetAttributes(ULONG sfgaoMask, ULONG *psfgaoAttribs) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Compare(MyIShellItem *psi, DWORD hint, int *piOrder) = 0;

#define DEFINE_IModalWindow_METHODS \
    virtual HRESULT STDMETHODCALLTYPE Show(HWND hwndOwner) = 0;

#define DEFINE_IShellItemFilter_METHODS \
    virtual HRESULT STDMETHODCALLTYPE IncludeItem(MyIShellItem *psi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetEnumFlagsForItem(MyIShellItem *psi, DWORD *pgrfFlags) = 0;

#define DEFINE_IEnumShellItems_METHODS \
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, MyIShellItem **rgelt, ULONG *pceltFetched) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Reset() = 0; \
    virtual HRESULT STDMETHODCALLTYPE Clone(MyIEnumShellItems **ppenum) = 0;

#define DEFINE_IFileDialog_METHODS \
    virtual HRESULT STDMETHODCALLTYPE SetFileTypes(UINT cFileTypes, const struct _COMDLG_FILTERSPEC *rgFilterSpec) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetFileTypeIndex(UINT iFileType) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetFileTypeIndex(UINT *piFileType) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Advise(IUnknown *pfde, DWORD *pdwCookie) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD dwCookie) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetOptions(DWORD fos) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetOptions(DWORD *pfos) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetDefaultFolder(MyIShellItem *psi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetFolder(MyIShellItem *psi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetFolder(MyIShellItem **ppsi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSelection(MyIShellItem **ppsi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetFileName(LPCWSTR pszName) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetFileName(LPWSTR *pszName) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetTitle(LPCWSTR pszTitle) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetOkButtonLabel(LPCWSTR pszText) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetFileNameLabel(LPCWSTR pszLabel) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetResult(MyIShellItem **ppsi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE AddPlace(MyIShellItem *psi, int fdap) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetDefaultExtension(LPCWSTR pszDefaultExtension) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Close(HRESULT hr) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetClientGuid(REFGUID guid) = 0; \
    virtual HRESULT STDMETHODCALLTYPE ClearClientData() = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetFilter(MyIShellItemFilter *pFilter) = 0;

#define DEFINE_IFileDialogEvents_METHODS \
    virtual HRESULT STDMETHODCALLTYPE OnFileOk(MyIFileDialog *pfd) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnFolderChanging(MyIFileDialog *pfd, MyIShellItem *psiFolder) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnFolderChange(MyIFileDialog *pfd) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnSelectionChange(MyIFileDialog *pfd) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnShareViolation(MyIFileDialog *pfd, MyIShellItem *psi, int *pResponse) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnTypeChange(MyIFileDialog *pfd) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnOverwrite(MyIFileDialog *pfd, MyIShellItem *psi, int *pResponse) = 0;

#define DEFINE_IFileOpenDialog_METHODS \
    virtual HRESULT STDMETHODCALLTYPE GetResults(MyIShellItemArray **ppenum) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetSelectedItems(MyIShellItemArray **ppsai) = 0;

#define DEFINE_IFileSaveDialog_METHODS \
    virtual HRESULT STDMETHODCALLTYPE SetSaveAsItem(MyIShellItem* psi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetProperties(IUnknown* pStore) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetCollectedProperties(IUnknown* pStore, BOOL fAppendDefault) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetProperties(IUnknown** ppStore) = 0; \
    virtual HRESULT STDMETHODCALLTYPE ApplyProperties(MyIShellItem* psi, IUnknown* pStore, HWND hwnd, IUnknown* pSink) = 0;

// Define or override specific interfaces

#if !defined(__IModalWindow_FWD_DEFINED__) && !defined(FORCE_MY_INTERFACE)
#define __IModalWindow_FWD_DEFINED__
DEFINE_INTERFACE(IModalWindow, IUnknown, DEFINE_IModalWindow_METHODS)
typedef IModalWindow MyIModalWindow;
#elif defined(TOGGLE_IModalWindow) || defined(FORCE_MY_INTERFACE)
#undef __IModalWindow_FWD_DEFINED__
DEFINE_MY_INTERFACE(MyIModalWindow, MyIModalWindow, IUnknown, DEFINE_IModalWindow_METHODS)
#endif // __IModalWindow_FWD_DEFINED__

#if !defined(__IShellItem_FWD_DEFINED__) && !defined(FORCE_MY_INTERFACE)
#define __IShellItem_FWD_DEFINED__
DEFINE_INTERFACE(IShellItem, IUnknown, DEFINE_IShellItem_METHODS)
typedef IShellItem MyIShellItem;
#elif defined(TOGGLE_IShellItem) || defined(FORCE_MY_INTERFACE)
#undef __IShellItem_FWD_DEFINED__
DEFINE_MY_INTERFACE(MyIShellItem, MyIShellItem, IUnknown, DEFINE_IShellItem_METHODS)
#endif // __IShellItem_FWD_DEFINED__

#if !defined(__IShellItemArray_FWD_DEFINED__) && !defined(FORCE_MY_INTERFACE)
#define __IShellItemArray_FWD_DEFINED__
DEFINE_INTERFACE(IShellItemArray, IUnknown, DEFINE_IShellItemArray_METHODS)
typedef IShellItemArray MyIShellItemArray;
#elif defined(TOGGLE_IShellItemArray) || defined(FORCE_MY_INTERFACE)
#undef __IShellItemArray_FWD_DEFINED__
DEFINE_MY_INTERFACE(MyIShellItemArray, MyIShellItemArray, IUnknown, DEFINE_IShellItemArray_METHODS)
#endif // __IShellItemArray_FWD_DEFINED__

#if !defined(__IShellItemFilter_FWD_DEFINED__) && !defined(FORCE_MY_INTERFACE)
#define __IShellItemFilter_FWD_DEFINED__
DEFINE_INTERFACE(IShellItemFilter, IUnknown, DEFINE_IShellItemFilter_METHODS)
typedef IShellItemFilter MyIShellItemFilter;
#elif defined(TOGGLE_IShellItemFilter) || defined(FORCE_MY_INTERFACE)
#undef __IShellItemFilter_FWD_DEFINED__
DEFINE_MY_INTERFACE(MyIShellItemFilter, MyIShellItemFilter, IUnknown, DEFINE_IShellItemFilter_METHODS)
#endif // __IShellItemFilter_FWD_DEFINED__

#if !defined(__IEnumShellItems_FWD_DEFINED__) && !defined(FORCE_MY_INTERFACE)
#define __IEnumShellItems_FWD_DEFINED__
DEFINE_INTERFACE(IEnumShellItems, IUnknown, DEFINE_IEnumShellItems_METHODS)
typedef IEnumShellItems MyIEnumShellItems;
#elif defined(TOGGLE_IEnumShellItems) || defined(FORCE_MY_INTERFACE)
#undef __IEnumShellItems_FWD_DEFINED__
DEFINE_MY_INTERFACE(MyIEnumShellItems, MyIEnumShellItems, IUnknown, DEFINE_IEnumShellItems_METHODS)
#endif // __IEnumShellItems_FWD_DEFINED__

#if !defined(__IFileDialog_FWD_DEFINED__) && !defined(FORCE_MY_INTERFACE)
#define __IFileDialog_FWD_DEFINED__
DEFINE_INTERFACE(IFileDialog, MyIModalWindow, DEFINE_IFileDialog_METHODS)
typedef IFileDialog MyIFileDialog;
#elif defined(TOGGLE_IFileDialog) || defined(FORCE_MY_INTERFACE)
#undef __IFileDialog_FWD_DEFINED__
DEFINE_MY_INTERFACE(MyIFileDialog, MyIFileDialog, MyIModalWindow, DEFINE_IFileDialog_METHODS)
#endif // __IFileDialog_FWD_DEFINED__

#if !defined(__IFileDialogEvents_FWD_DEFINED__) && !defined(FORCE_MY_INTERFACE)
#define __IFileDialogEvents_FWD_DEFINED__
DEFINE_INTERFACE(IFileDialogEvents, IUnknown, DEFINE_IFileDialogEvents_METHODS)
typedef IFileDialogEvents MyIFileDialogEvents;
#elif defined(TOGGLE_IFileDialogEvents) || defined(FORCE_MY_INTERFACE)
#undef __IFileDialogEvents_FWD_DEFINED__
DEFINE_MY_INTERFACE(MyIFileDialogEvents, MyIFileDialogEvents, IUnknown, DEFINE_IFileDialogEvents_METHODS)
#endif // __IFileDialogEvents_FWD_DEFINED__

#if !defined(__IFileOpenDialog_FWD_DEFINED__) && !defined(FORCE_MY_INTERFACE)
#define __IFileOpenDialog_FWD_DEFINED__
DEFINE_INTERFACE(IFileOpenDialog, IFileDialog, DEFINE_IFileOpenDialog_METHODS)
typedef IFileOpenDialog MyIFileOpenDialog;
#elif defined(TOGGLE_IFileOpenDialog) || defined(FORCE_MY_INTERFACE)
#undef __IFileOpenDialog_FWD_DEFINED__
DEFINE_MY_INTERFACE(MyIFileOpenDialog, MyIFileOpenDialog, MyIFileDialog, DEFINE_IFileOpenDialog_METHODS)
#endif // __IFileOpenDialog_FWD_DEFINED__

#if !defined(__IFileSaveDialog_FWD_DEFINED__) && !defined(FORCE_MY_INTERFACE)
#define __IFileSaveDialog_FWD_DEFINED__
DEFINE_INTERFACE(IFileSaveDialog, IFileDialog, DEFINE_IFileSaveDialog_METHODS)
typedef IFileSaveDialog MyIFileSaveDialog;
#elif defined(TOGGLE_IFileSaveDialog) || defined(FORCE_MY_INTERFACE)
#undef __IFileSaveDialog_FWD_DEFINED__
DEFINE_MY_INTERFACE(MyIFileSaveDialog, MyIFileSaveDialog, MyIFileDialog, DEFINE_IFileSaveDialog_METHODS)
#endif // __IFileSaveDialog_FWD_DEFINED__

// COM constants
#ifndef __shobjidl_h__

static const IID IID_IFileOpenDialog = {0xd57c7288, 0xd4ad, 0x4768, {0xbe, 0x02, 0x9d, 0x96, 0x95, 0x32, 0xd9, 0x60}};
static const IID IID_IFileSaveDialog = {0x84bccd23, 0x5fde, 0x4cdb, {0xae, 0xa4, 0xaf, 0x64, 0xb8, 0x3d, 0x78, 0xab}};
static const IID IID_IShellItem = {0x43826d1e, 0xe718, 0x42ee, {0xbc, 0x55, 0xa1, 0xe2, 0x61, 0xc3, 0x7b, 0xfe}};
static const IID IID_IShellItemArray = {0xb63ea76d, 0x1f85, 0x456f, {0xa1, 0x9c, 0x48, 0x15, 0x9e, 0xfa, 0x85, 0x8b}};
static const IID IID_IFileDialogEvents = {0x973510db, 0x7d7f, 0x452b, {0x89, 0x75, 0x74, 0xa8, 0x58, 0x28, 0xd3, 0x54}};
static const CLSID CLSID_FileOpenDialog = {0xdc1c5a9c, 0xe88a, 0x4dde, {0xa5, 0xa1, 0x60, 0xf8, 0x2a, 0x20, 0xae, 0xf7}};
static const CLSID CLSID_FileSaveDialog = {0xc0b4e2f3, 0xba21, 0x4773, {0x8d, 0xba, 0x33, 0x5e, 0xc9, 0x46, 0xeb, 0x8b}};

enum FDE_SHAREVIOLATION_RESPONSE : DWORD {
    FDESVR_DEFAULT = 0x00000000,
    FDESVR_ACCEPT = 0x00000001,
    FDESVR_REFUSE = 0x00000002
};

typedef FDE_SHAREVIOLATION_RESPONSE FDE_OVERWRITE_RESPONSE;

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

// Enum definitions and other constants
enum SIGDN : DWORD {
    SIGDN_NORMALDISPLAY = 0x00000000,
    SIGDN_PARENTRELATIVEPARSING = 0x80018001,
    SIGDN_PARENTRELATIVEFORADDRESSBAR = 0x8001c001,
    SIGDN_DESKTOPABSOLUTEPARSING = 0x80028000,
    SIGDN_PARENTRELATIVEEDITING = 0x80031001,
    SIGDN_DESKTOPABSOLUTEEDITING = 0x8004c000,
    SIGDN_FILESYSPATH = 0x80058000,
    SIGDN_URL = 0x80068000
};

#endif // __shobjidl_h__

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
LPCWSTR string_to_LPCWSTR(const std::wstring& s);  // Helper to convert std::wstring to LPCWSTR
std::wstring LPCWSTR_to_string(LPCWSTR s);  // Helper to convert LPCWSTR to std::wstring

// Function pointer types for the dynamically loaded functions
typedef HRESULT (STDMETHODCALLTYPE *PFN_CoInitialize)(LPVOID);
typedef void (STDMETHODCALLTYPE *PFN_CoUninitialize)();
typedef HRESULT (STDMETHODCALLTYPE *PFN_CoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
typedef HRESULT (STDMETHODCALLTYPE *PFN_SHCreateItemFromParsingName)(LPCWSTR, LPVOID, REFIID, void**);
typedef void (STDMETHODCALLTYPE *PFN_CoTaskMemFree)(LPVOID);

// Function declarations
void FreeCOMFunctionPointers(COMFunctionPointers& comFuncPtrs);
void createFileDialog(COMFunctionPointers& comFuncs, MyIFileDialog** ppFileDialog, bool isSaveDialog);
void showDialog(COMFunctionPointers& comFuncs, MyIFileDialog* pFileOpenDialog, HWND hwndOwner = NULL);
std::vector<std::wstring> getFileDialogResults(COMFunctionPointers& comFuncs, MyIFileOpenDialog* pFileOpenDialog);
void configureFileDialog(COMFunctionPointers& comFuncs, MyIFileDialog* pFileDialog, const std::vector<COMDLG_FILTERSPEC>& filters, const std::wstring& defaultFolder, DWORD options, bool forceFileSystem = false, bool allowMultiselect = false);

LPCWSTR string_to_LPCWSTR(const std::wstring& s);  // Helper to convert std::wstring to LPCWSTR
std::wstring LPCWSTR_to_string(LPCWSTR s);  // Helper to convert LPCWSTR to std::wstring

#endif // IFILEDIALOG_H
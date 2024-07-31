#ifndef IFILEDIALOG_H
#define IFILEDIALOG_H

#include <vector>
#include <string>
#include "ProjWinUtils.h"

// **
// **
// Toggle debugging tests
//
// Individual toggles. Default implementation is all commented.
// Uncommented - This literal file header you're in, will override that interface definition in the shobjidl.h
// Commented - Default implementation. When implementing IFileDialog, most users just #include shobjidl.h and be done with it. That is what happens when you comment everything.
#define __IModalWindow_FWD_DEFINED__
#define __IShellItemArray_INTERFACE_DEFINED__  // IShellItemArray. The root of all evil. Took ages to track the GetCount access violation here.
#define __IShellItem_INTERFACE_DEFINED__
#define __IShellItemFilter_FWD_DEFINED__
#define __IEnumShellItems_FWD_DEFINED__
#define __IFileDialog_FWD_DEFINED__ // requires __IModalWindow_FWD_DEFINED__ defined.
#define __IFileDialogEvents_FWD_DEFINED__
#define __IFileOpenDialog_FWD_DEFINED__ // requires __IFileDialog_FWD_DEFINED__ defined.
#define __IFileSaveDialog_FWD_DEFINED__ // requires __IFileDialog_FWD_DEFINED__ defined.
// Other Toggles
//
// Provided that the above defines are ALL uncommented, there is no functional difference when
// commenting this next line vs uncommenting. This define is what controls whether shobjidl should be imported.
// At this point after a bit of testing, I believe we have everything required from that file in here? Not sure, play around with it and find out.
#define __shobjidl_h__  // Should we include shobjidl.h at all? (commented=not included)
//
// shlwapi. Defines IUnknown, interface, DWORD, and a few others.
// The toggle is not currently functional.
#define _INC_SHLWAPI  // Should we include shlwapi.h at all? (commented=not included)
// **
// **
#if !defined(_INC_SHLWAPI)
#include <shlwapi.h>

#else

#ifndef interface
#define interface struct
#endif

// TODO: IUnknown, DWORD, etc
//#define interface struct

#endif // _INC_SHLWAPI

// Macro to simplify the interface definitions
#define DEFINE_INTERFACE(iface, base, methods) \
    struct iface : public base { \
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
    virtual HRESULT STDMETHODCALLTYPE GetItemAt(DWORD dwIndex, IShellItem **ppsi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE EnumItems(IEnumShellItems **ppenumShellItems) = 0;

#define DEFINE_IShellItem_METHODS \
    virtual HRESULT STDMETHODCALLTYPE BindToHandler(IUnknown *pbc, REFGUID bhid, REFIID riid, void **ppv) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetParent(IShellItem **ppsi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetDisplayName(int sigdnName, LPWSTR *ppszName) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetAttributes(ULONG sfgaoMask, ULONG *psfgaoAttribs) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Compare(IShellItem *psi, DWORD hint, int *piOrder) = 0;

#define DEFINE_IModalWindow_METHODS \
    virtual HRESULT STDMETHODCALLTYPE Show(HWND hwndOwner) = 0;

#define DEFINE_IShellItemFilter_METHODS \
    virtual HRESULT STDMETHODCALLTYPE IncludeItem(IShellItem *psi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetEnumFlagsForItem(IShellItem *psi, DWORD *pgrfFlags) = 0;

#define DEFINE_IEnumShellItems_METHODS \
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, IShellItem **rgelt, ULONG *pceltFetched) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Reset() = 0; \
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumShellItems **ppenum) = 0;

#define DEFINE_IFileDialog_METHODS \
    virtual HRESULT STDMETHODCALLTYPE SetFileTypes(UINT cFileTypes, const struct _COMDLG_FILTERSPEC *rgFilterSpec) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetFileTypeIndex(UINT iFileType) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetFileTypeIndex(UINT *piFileType) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Advise(IUnknown *pfde, DWORD *pdwCookie) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD dwCookie) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetOptions(DWORD fos) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetOptions(DWORD *pfos) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetDefaultFolder(IShellItem *psi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetFolder(IShellItem *psi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetFolder(IShellItem **ppsi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSelection(IShellItem **ppsi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetFileName(LPCWSTR pszName) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetFileName(LPWSTR *pszName) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetTitle(LPCWSTR pszTitle) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetOkButtonLabel(LPCWSTR pszText) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetFileNameLabel(LPCWSTR pszLabel) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetResult(IShellItem **ppsi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE AddPlace(IShellItem *psi, int fdap) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetDefaultExtension(LPCWSTR pszDefaultExtension) = 0; \
    virtual HRESULT STDMETHODCALLTYPE Close(HRESULT hr) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetClientGuid(REFGUID guid) = 0; \
    virtual HRESULT STDMETHODCALLTYPE ClearClientData() = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetFilter(IShellItemFilter *pFilter) = 0;

#define DEFINE_IFileDialogEvents_METHODS \
    virtual HRESULT STDMETHODCALLTYPE OnFileOk(IFileDialog *pfd) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnFolderChanging(IFileDialog *pfd, IShellItem *psiFolder) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnFolderChange(IFileDialog *pfd) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnSelectionChange(IFileDialog *pfd) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnShareViolation(IFileDialog *pfd, IShellItem *psi, int *pResponse) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnTypeChange(IFileDialog *pfd) = 0; \
    virtual HRESULT STDMETHODCALLTYPE OnOverwrite(IFileDialog *pfd, IShellItem *psi, int *pResponse) = 0;

#define DEFINE_IFileOpenDialog_METHODS \
    virtual HRESULT STDMETHODCALLTYPE GetResults(IShellItemArray **ppenum) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetSelectedItems(IShellItemArray **ppsai) = 0;

#define DEFINE_IFileSaveDialog_METHODS \
    virtual HRESULT STDMETHODCALLTYPE SetSaveAsItem(IShellItem* psi) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetProperties(IUnknown* pStore) = 0; \
    virtual HRESULT STDMETHODCALLTYPE SetCollectedProperties(IUnknown* pStore, BOOL fAppendDefault) = 0; \
    virtual HRESULT STDMETHODCALLTYPE GetProperties(IUnknown** ppStore) = 0; \
    virtual HRESULT STDMETHODCALLTYPE ApplyProperties(IShellItem* psi, IUnknown* pStore, HWND hwnd, IUnknown* pSink) = 0;

// Toggle definitions for interface forward declaration
#ifdef __IModalWindow_FWD_DEFINED__
interface IModalWindow;  // Forward declaration of the interface
#define __IModalWindow_INTERFACE_DEFINED__
typedef interface IModalWindow IModalWindow;
DEFINE_INTERFACE(IModalWindow, IUnknown, DEFINE_IModalWindow_METHODS)
#endif // __IModalWindow_FWD_DEFINED__

#define __IShellItem_FWD_DEFINED__
interface IShellItem;  // Forward declaration of the interface

#define __IShellItemArray_FWD_DEFINED__
interface IShellItemArray;  // Forward declaration of the interface

#ifdef __IEnumShellItems_FWD_DEFINED__
interface IEnumShellItems;  // Forward declaration of the interface
#define __IEnumShellItems_INTERFACE_DEFINED__
typedef interface IEnumShellItems IEnumShellItems;
DEFINE_INTERFACE(IEnumShellItems, IUnknown, DEFINE_IEnumShellItems_METHODS)
#endif // __IEnumShellItems_FWD_DEFINED__

#ifdef __IShellItemArray_INTERFACE_DEFINED__
typedef interface IShellItemArray IShellItemArray;
DEFINE_INTERFACE(IShellItemArray, IUnknown, DEFINE_IShellItemArray_METHODS)
#endif // __IShellItemArray_INTERFACE_DEFINED__



typedef DWORD SICHINTF;
#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)
#ifdef __IShellItem_INTERFACE_DEFINED__
typedef /* [v1_enum] */
enum _SIGDN
    {
        SIGDN_NORMALDISPLAY	= 0,
        SIGDN_PARENTRELATIVEPARSING	= ( int  )0x80018001,
        SIGDN_DESKTOPABSOLUTEPARSING	= ( int  )0x80028000,
        SIGDN_PARENTRELATIVEEDITING	= ( int  )0x80031001,
        SIGDN_DESKTOPABSOLUTEEDITING	= ( int  )0x8004c000,
        SIGDN_FILESYSPATH	= ( int  )0x80058000,
        SIGDN_URL	= ( int  )0x80068000,
        SIGDN_PARENTRELATIVEFORADDRESSBAR	= ( int  )0x8007c001,
        SIGDN_PARENTRELATIVE	= ( int  )0x80080001,
        SIGDN_PARENTRELATIVEFORUI	= ( int  )0x80094001
    } 	SIGDN;
/* [v1_enum] */
enum _SICHINTF
    {
        SICHINT_DISPLAY	= 0,
        SICHINT_ALLFIELDS	= ( int  )0x80000000,
        SICHINT_CANONICAL	= 0x10000000,
        SICHINT_TEST_FILESYSPATH_IF_NOT_EQUAL	= 0x20000000
    } ;
typedef interface IShellItem IShellItem;
struct __declspec(uuid("43826D1E-E718-42EE-BC55-A1E261C37BFE")) IShellItem : public IUnknown {
    DEFINE_IShellItem_METHODS
};
static const IID IID_IShellItem = {0x43826d1e, 0xe718, 0x42ee, {0xbc, 0x55, 0xa1, 0xe2, 0x61, 0xc3, 0x7b, 0xfe}};
#endif // __IShellItem_INTERFACE_DEFINED__

#ifdef __IShellItemFilter_FWD_DEFINED__
#define __IShellItemFilter_INTERFACE_DEFINED__
typedef interface IShellItemFilter IShellItemFilter;
DEFINE_INTERFACE(IShellItemFilter, IUnknown, DEFINE_IShellItemFilter_METHODS)
#endif // __IShellItemFilter_FWD_DEFINED__

#ifdef __IFileDialog_FWD_DEFINED__
interface IFileDialog;  // Forward declaration of the interface
static const IID IID_IFileDialog = {0x4292C689, 0x9298, 0x4A37, {0xB8, 0xF0, 0x03, 0x83, 0x5B, 0x7B, 0x6E, 0xE6}};


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
#define __IFileDialog_INTERFACE_DEFINED__
typedef interface IFileDialog IFileDialog;
DEFINE_INTERFACE(IFileDialog, IModalWindow, DEFINE_IFileDialog_METHODS)
#endif // __IFileDialog_FWD_DEFINED__

#ifdef __IFileDialogEvents_FWD_DEFINED__
interface IFileDialogEvents;  // Forward declaration of the interface
static const IID IID_IFileDialogEvents = {0x973510db, 0x7d7f, 0x452b, {0x89, 0x75, 0x74, 0xa8, 0x58, 0x28, 0xd3, 0x54}};
#define __IFileDialogEvents_INTERFACE_DEFINED__
typedef interface IFileDialogEvents IFileDialogEvents;
DEFINE_INTERFACE(IFileDialogEvents, IUnknown, DEFINE_IFileDialogEvents_METHODS)
#endif // __IFileDialogEvents_FWD_DEFINED__

#ifdef __IFileOpenDialog_FWD_DEFINED__
interface IFileOpenDialog;  // Forward declaration of the interface
static const IID IID_IFileOpenDialog = {0xd57c7288, 0xd4ad, 0x4768, {0xbe, 0x02, 0x9d, 0x96, 0x95, 0x32, 0xd9, 0x60}};
#define __IFileOpenDialog_INTERFACE_DEFINED__
typedef interface IFileOpenDialog IFileOpenDialog;
DEFINE_INTERFACE(IFileOpenDialog, IFileDialog, DEFINE_IFileOpenDialog_METHODS)
#endif // __IFileOpenDialog_FWD_DEFINED__

#ifdef __IFileSaveDialog_FWD_DEFINED__
interface IFileSaveDialog;  // Forward declaration of the interface
static const IID IID_IFileSaveDialog = {0x84bccd23, 0x5fde, 0x4cdb, {0xae, 0xa4, 0xaf, 0x64, 0xb8, 0x3d, 0x78, 0xab}};
#define __IFileSaveDialog_INTERFACE_DEFINED__
typedef interface IFileSaveDialog IFileSaveDialog;
DEFINE_INTERFACE(IFileSaveDialog, IFileDialog, DEFINE_IFileSaveDialog_METHODS)
#endif // __IFileSaveDialog_FWD_DEFINED__

#if !defined(__shobjidl_h__)
#include <ShObjIdl.h>
#else // __shobjidl_h__

// Other COM constants
static const IID IID_IShellItemArray = {0xb63ea76d, 0x1f85, 0x456f, {0xa1, 0x9c, 0x48, 0x15, 0x9e, 0xfa, 0x85, 0x8b}};
static const CLSID CLSID_FileDialog = {0x3D9C8F03, 0x50D4, 0x4E40, {0xBB, 0x11, 0x70, 0xE7, 0x4D, 0x3F, 0x10, 0xF3}};
static const CLSID CLSID_FileOpenDialog = {0xdc1c5a9c, 0xe88a, 0x4dde, {0xa5, 0xa1, 0x60, 0xf8, 0x2a, 0x20, 0xae, 0xf7}};
static const CLSID CLSID_FileSaveDialog = {0xc0b4e2f3, 0xba21, 0x4773, {0x8d, 0xba, 0x33, 0x5e, 0xc9, 0x46, 0xeb, 0x8b}};

enum FDE_SHAREVIOLATION_RESPONSE : DWORD {
    FDESVR_DEFAULT = 0x00000000,
    FDESVR_ACCEPT = 0x00000001,
    FDESVR_REFUSE = 0x00000002
};

typedef FDE_SHAREVIOLATION_RESPONSE FDE_OVERWRITE_RESPONSE;

typedef int GETPROPERTYSTOREFLAGS;
typedef ULONG SFGAOF;
#define SFGAO_FILESYSTEM 0x40000000
#define SFGAO_FOLDER 0x20000000
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

typedef struct _COMDLG_FILTERSPEC
    {
    /* [string] */ LPCWSTR pszName;
    /* [string] */ LPCWSTR pszSpec;
    } 	COMDLG_FILTERSPEC;

#endif // __shobjidl_h__

// Function pointer types for the dynamically loaded functions
typedef HRESULT (STDMETHODCALLTYPE *PFN_CoInitialize)(LPVOID);
typedef void (STDMETHODCALLTYPE *PFN_CoUninitialize)();
typedef HRESULT (STDMETHODCALLTYPE *PFN_CoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
typedef HRESULT (STDMETHODCALLTYPE *PFN_SHCreateItemFromParsingName)(LPCWSTR, LPVOID, REFIID, void**);
typedef void (STDMETHODCALLTYPE *PFN_CoTaskMemFree)(LPVOID);

// Function declarations
void createFileDialog(COMFunctionPointers& comFuncs, IFileDialog** ppFileDialog, int isSaveDialog);
void showDialog(COMFunctionPointers& comFuncs, IFileDialog* pFileOpenDialog, HWND hwndOwner = NULL);
IShellItem* createShellItem(COMFunctionPointers& comFuncs, const std::wstring& path);
std::vector<std::wstring> getFilePathsFromShellItemArray(IShellItemArray* pItemArray);
std::vector<std::wstring> getFileDialogResults(COMFunctionPointers& comFuncs, IFileOpenDialog* pFileOpenDialog);
void configureFileDialog(COMFunctionPointers& comFuncs, IFileDialog* pFileDialog, const std::vector<COMDLG_FILTERSPEC>& filters, const std::wstring& defaultFolder, DWORD options, bool forceFileSystem = false, bool allowMultiselect = false);

LPCWSTR string_to_LPCWSTR(const std::wstring& s);  // Helper to convert std::wstring to LPCWSTR
std::wstring LPCWSTR_to_string(LPCWSTR s);  // Helper to convert LPCWSTR to std::wstring

#endif // IFILEDIALOG_H

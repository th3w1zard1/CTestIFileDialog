#ifndef RAWWINAPI_H
#define RAWWINAPI_H

#include <guiddef.h> // for REFIID and other GUID related types
#include <intrin.h>  // for _InterlockedIncrement and _InterlockedDecrement
#include <iostream>  // for std::cerr
#include <cstring>   // for strcmp
#include <cstdint>   // for uintptr_t

// Manually defined GUID and IID structures and related definitions
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;
#endif // GUID_DEFINED

typedef GUID IID;
typedef GUID CLSID;
#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
typedef const GUID& REFGUID;
typedef REFGUID REFCLSID;
#endif // _REFGUID_DEFINED

// Windows API types
typedef signed long LONG;
typedef unsigned long ULONG;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef const char* LPCSTR;
typedef int BOOL;
#ifndef _COMPARE_WIN_API_TEST_
typedef void* HWND;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef void* FARPROC;
typedef void* LPUNKNOWN; // Added definition for LPUNKNOWN
#endif

typedef uintptr_t ULONG_PTR;
typedef unsigned char BYTE;
typedef unsigned short WORD;

// HRESULT definitions
#ifndef HRESULT
#define HRESULT long
#endif // HRESULT

#ifndef S_OK
#define S_OK ((HRESULT)0L)
#endif // S_OK

// Define _HRESULT_TYPEDEF_ if not already defined
#ifndef _HRESULT_TYPEDEF_
#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
#endif // _HRESULT_TYPEDEF_

#ifndef E_NOINTERFACE
#define E_NOINTERFACE _HRESULT_TYPEDEF_(0x80004002L)
#endif // E_NOINTERFACE

#ifndef FAILED
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif // FAILED

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif // SUCCEEDED

#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE __stdcall
#endif // STDMETHODCALLTYPE

#ifndef WINAPI
#define WINAPI __stdcall
#endif // WINAPI

#define CLSCTX_INPROC_SERVER 0x1

#define IMAGE_DIRECTORY_ENTRY_EXPORT 0

typedef ULONG_PTR SIZE_T, *PSIZE_T;
#ifndef _COMPARE_WIN_API_TEST_
typedef struct _MEMORY_BASIC_INFORMATION {
    LPVOID  BaseAddress;
    LPVOID  AllocationBase;
    DWORD   AllocationProtect;
    SIZE_T  RegionSize;
    DWORD   State;
    DWORD   Protect;
    DWORD   Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

typedef struct _IMAGE_DOS_HEADER {
    WORD e_magic;
    WORD e_cblp;
    WORD e_cp;
    WORD e_crlc;
    WORD e_cparhdr;
    WORD e_minalloc;
    WORD e_maxalloc;
    WORD e_ss;
    WORD e_sp;
    WORD e_csum;
    WORD e_ip;
    WORD e_cs;
    WORD e_lfarlc;
    WORD e_ovno;
    WORD e_res[4];
    WORD e_oemid;
    WORD e_oeminfo;
    WORD e_res2[10];
    LONG e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
    WORD Machine;
    WORD NumberOfSections;
    DWORD TimeDateStamp;
    DWORD PointerToSymbolTable;
    DWORD NumberOfSymbols;
    WORD SizeOfOptionalHeader;
    WORD Characteristics;
} IMAGE_FILE_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER {
    WORD Magic;
    BYTE MajorLinkerVersion;
    BYTE MinorLinkerVersion;
    DWORD SizeOfCode;
    DWORD SizeOfInitializedData;
    DWORD SizeOfUninitializedData;
    DWORD AddressOfEntryPoint;
    DWORD BaseOfCode;
    DWORD BaseOfData;
    DWORD ImageBase;
    DWORD SectionAlignment;
    DWORD FileAlignment;
    WORD MajorOperatingSystemVersion;
    WORD MinorOperatingSystemVersion;
    WORD MajorImageVersion;
    WORD MinorImageVersion;
    WORD MajorSubsystemVersion;
    WORD MinorSubsystemVersion;
    DWORD Win32VersionValue;
    DWORD SizeOfImage;
    DWORD SizeOfHeaders;
    DWORD CheckSum;
    WORD Subsystem;
    WORD DllCharacteristics;
    DWORD SizeOfStackReserve;
    DWORD SizeOfStackCommit;
    DWORD SizeOfHeapReserve;
    DWORD SizeOfHeapCommit;
    DWORD LoaderFlags;
    DWORD NumberOfRvaAndSizes;
    struct _IMAGE_DATA_DIRECTORY {
        DWORD VirtualAddress;
        DWORD Size;
    } DataDirectory[16];
} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_NT_HEADERS {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER OptionalHeader;
} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;

typedef struct _IMAGE_EXPORT_DIRECTORY {
    DWORD Characteristics;
    DWORD TimeDateStamp;
    WORD MajorVersion;
    WORD MinorVersion;
    DWORD Name;
    DWORD Base;
    DWORD NumberOfFunctions;
    DWORD NumberOfNames;
    DWORD AddressOfFunctions;
    DWORD AddressOfNames;
    DWORD AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;
#endif
#ifndef IMAGE_DIRECTORY_ENTRY_EXPORT
#define IMAGE_DIRECTORY_ENTRY_EXPORT 0
#endif

// Interlocked functions
LONG InterlockedIncrement(LONG volatile *Addend);
LONG InterlockedDecrement(LONG volatile *Addend);

// Function pointer types for dynamically loaded functions
#ifndef _CUSTOM_WIN_PROTOTYPES_
#define _CUSTOM_WIN_PROTOTYPES_
typedef HMODULE (WINAPI *PFN_LoadLibraryW)(LPCWSTR lpLibFileName);
typedef BOOL (WINAPI *PFN_FreeLibrary)(HMODULE hLibModule);
typedef FARPROC (WINAPI *PFN_GetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
#endif

typedef wchar_t WCHAR;
typedef _Null_terminated_ WCHAR *NWPSTR, *LPWSTR, *PWSTR;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
#ifndef _UNICODE_STRING_DEFINED_
#define _UNICODE_STRING_DEFINED_
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
#endif

typedef void * HANDLE;
typedef HANDLE *PHANDLE;
typedef void *PVOID;
// Add these type definitions to RawWinAPI.h
typedef LONG NTSTATUS;  // Define NTSTATUS as a LONG

// Define ANSI_STRING structure
typedef char CHAR;
typedef CHAR *PCHAR, *LPCH, *PCH;
typedef struct _ANSI_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR  Buffer;
} ANSI_STRING, *PANSI_STRING;
typedef NTSTATUS(WINAPI *PFN_LdrLoadDll)(PWSTR DllPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PHANDLE DllHandle);
typedef NTSTATUS(WINAPI *PFN_LdrGetProcedureAddress)(HMODULE ModuleHandle, PANSI_STRING FunctionName, WORD Oridinal, PVOID* FunctionAddress);
typedef NTSTATUS(WINAPI *PFN_LdrUnloadDll)(HANDLE DllHandle);

// Define REFIID, HRESULT, and other necessary types
#ifndef _REFIID_DEFINED
#define _REFIID_DEFINED
typedef GUID REFIID;
#endif // _REFIID_DEFINED

// Function prototypes
#ifndef _COMPARE_WIN_API_TEST_

SIZE_T WINAPI VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
HMODULE LoadLibraryW(LPCWSTR lpLibFileName);
BOOL FreeLibrary(HMODULE hLibModule);
FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

// Helper functions for finding exported functions
HMODULE findModuleBase(void *ptr);
void *findExportedFunction(HMODULE mod, const char *funcName);

// IUnknown interface definition
class IUnknown {
private:
    ULONG refCount;
public:
    IUnknown();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
};

#define RESTRICTED_POINTER *
typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY, *RESTRICTED_POINTER PRLIST_ENTRY;
#endif // _COMPARE_WIN_API_TEST_

// Define IID_IUnknown
#ifndef _IID_IUnknown_
static const IID IID_IUnknown = {0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
#endif // _IID_IUnknown_

#endif // RAWWINAPI_H

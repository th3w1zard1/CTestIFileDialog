#include "RawWinAPI.h"
#include <cstring>

// Access the PEB (Process Environment Block)
#ifdef _WIN64
#define NtCurrentPeb() ((PPEB)__readgsqword(0x60))
#else
#define NtCurrentPeb() ((PPEB)__readfsdword(0x30))
#endif

typedef void *PVOID;
typedef BYTE UCHAR;
typedef unsigned short USHORT;
typedef struct _PEB_LDR_DATA {
    ULONG Length;
    UCHAR Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB {
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    UCHAR BitField;
    PVOID Mutant;
    PVOID ImageBaseAddress;
    PPEB_LDR_DATA Ldr;
    // Other fields are omitted as they are not needed
} PEB, *PPEB;

typedef wchar_t WCHAR;
typedef _Null_terminated_ WCHAR *NWPSTR, *LPWSTR, *PWSTR;

typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    LIST_ENTRY HashLinks;
    PVOID SectionPointer;
    ULONG CheckSum;
    ULONG TimeDateStamp;
    PVOID LoadedImports;
    PVOID EntryPointActivationContext;
    PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

#define CONTAINING_RECORD(address, type, field) ((type *)((PCHAR)(address) - (ULONG_PTR)(&((type *)0)->field)))

LONG InterlockedIncrement(LONG volatile *Addend) {
    return _InterlockedIncrement(Addend);
}

LONG InterlockedDecrement(LONG volatile *Addend) {
    return _InterlockedDecrement(Addend);
}

HMODULE LoadLibraryW(LPCWSTR lpLibFileName) {
    static PFN_LoadLibraryW pLoadLibraryW = NULL;
    if (!pLoadLibraryW) {
        HMODULE hKernel32 = findModuleBase((void*)&LoadLibraryW);
        pLoadLibraryW = (PFN_LoadLibraryW)findExportedFunction(hKernel32, "LoadLibraryW");
    }
    return pLoadLibraryW(lpLibFileName);
}

BOOL FreeLibrary(HMODULE hLibModule) {
    static PFN_FreeLibrary pFreeLibrary = NULL;
    if (!pFreeLibrary) {
        HMODULE hKernel32 = findModuleBase((void*)&FreeLibrary);
        pFreeLibrary = (PFN_FreeLibrary)findExportedFunction(hKernel32, "FreeLibrary");
    }
    return pFreeLibrary(hLibModule);
}

FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
    static PFN_GetProcAddress pGetProcAddress = NULL;
    if (!pGetProcAddress) {
        HMODULE hKernel32 = findModuleBase((void*)&GetProcAddress);
        pGetProcAddress = (PFN_GetProcAddress)findExportedFunction(hKernel32, "GetProcAddress");
    }
    return pGetProcAddress(hModule, lpProcName);
}

HMODULE MyLdrGetDllHandle(const wchar_t* dllName) {
    PPEB pPeb = NtCurrentPeb();
    PPEB_LDR_DATA pLdr = pPeb->Ldr;
    PLIST_ENTRY pModuleList = &pLdr->InLoadOrderModuleList;
    PLIST_ENTRY pEntry = pModuleList->Flink;

    while (pEntry != pModuleList) {
        PLDR_DATA_TABLE_ENTRY pLdrEntry = CONTAINING_RECORD(pEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
        if (_wcsicmp(pLdrEntry->BaseDllName.Buffer, dllName) == 0) {
            return (HMODULE)pLdrEntry->DllBase;
        }
        pEntry = pEntry->Flink;
    }
    return NULL;
}

void* MyLdrGetProcedureAddress(HMODULE hModule, const char* functionName) {
    if (!hModule || !functionName) {
        return NULL;
    }

    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);

    PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)hModule + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    DWORD* pFunctionNames = (DWORD*)((BYTE*)hModule + pExportDir->AddressOfNames);
    WORD* pNameOrdinals = (WORD*)((BYTE*)hModule + pExportDir->AddressOfNameOrdinals);
    DWORD* pFunctionAddresses = (DWORD*)((BYTE*)hModule + pExportDir->AddressOfFunctions);

    for (DWORD i = 0; i < pExportDir->NumberOfNames; i++) {
        const char* pFuncName = (const char*)((BYTE*)hModule + pFunctionNames[i]);
        if (strcmp(functionName, pFuncName) == 0) {
            WORD ordinal = pNameOrdinals[i];
            return (void*)((BYTE*)hModule + pFunctionAddresses[ordinal]);
        }
    }
    return NULL;
}

SIZE_T WINAPI VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength) {
    if (dwLength < sizeof(MEMORY_BASIC_INFORMATION)) {
        return 0; // Not enough space in lpBuffer
    }

    // Load NTDLL
    HMODULE hNtdll = MyLdrGetDllHandle(L"ntdll.dll");
    if (!hNtdll) {
        std::cerr << "Failed to load NTDLL" << std::endl;
        return 0;
    }

    // Get the address of VirtualQuery
    auto pVirtualQuery = (decltype(&VirtualQuery))MyLdrGetProcedureAddress(hNtdll, "VirtualQuery");
    if (!pVirtualQuery) {
        std::cerr << "Failed to get VirtualQuery address" << std::endl;
        return 0;
    }

    return pVirtualQuery(lpAddress, lpBuffer, dwLength);
}

HMODULE findModuleBase(void *ptr) {
    ULONG_PTR addr = (ULONG_PTR)ptr;
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery((LPCVOID)addr, &mbi, sizeof(mbi));
    return (HMODULE)mbi.AllocationBase;
}

void *findExportedFunction(HMODULE mod, const char *funcName) {
    PIMAGE_DOS_HEADER idh = (PIMAGE_DOS_HEADER)mod;
    PIMAGE_NT_HEADERS inh = (PIMAGE_NT_HEADERS)((BYTE*)mod + idh->e_lfanew);
    PIMAGE_EXPORT_DIRECTORY ied = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)mod + inh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    DWORD *names = (DWORD*)((BYTE*)mod + ied->AddressOfNames);
    WORD *ordinals = (WORD*)((BYTE*)mod + ied->AddressOfNameOrdinals);
    DWORD *functions = (DWORD*)((BYTE*)mod + ied->AddressOfFunctions);

    for (DWORD i = 0; i < ied->NumberOfNames; i++) {
        if (strcmp(funcName, (char*)mod + names[i]) == 0) {
            return (void*)((BYTE*)mod + functions[ordinals[i]]);
        }
    }
    return NULL;
}

IUnknown::IUnknown() : refCount(1) {}

HRESULT STDMETHODCALLTYPE IUnknown::QueryInterface(REFIID riid, void **ppvObject) {
    if (riid == IID_IUnknown) {
        *ppvObject = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }
    *ppvObject = NULL;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE IUnknown::AddRef() {
    return InterlockedIncrement((LONG*)&refCount);
}

ULONG STDMETHODCALLTYPE IUnknown::Release() {
    ULONG newCount = InterlockedDecrement((LONG*)&refCount);
    if (newCount == 0) {
        delete this;
    }
    return newCount;
}

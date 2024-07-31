#include <vector>
#include <string>
#include <random>
#include <stdexcept>
#include <windows.h>
#include "ProjUtil.h"

std::wstring trim(const std::wstring& str) {
    auto start = std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    auto end = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base();
    return (start < end) ? std::wstring(start, end) : std::wstring();
}

// Helper function to get recent files paths from registry
std::vector<std::wstring> getRecentFilesPaths() {
    std::vector<std::wstring> recentPaths;
    HKEY hKey;

    // Open the registry key
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\RecentDocs", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t valueName[256];
        DWORD valueNameSize = sizeof(valueName) / sizeof(valueName[0]);
        DWORD index = 0;

        // Enumerate the values in the registry key
        while (RegEnumValueW(hKey, index++, valueName, &valueNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            std::wstring path(valueName);
            recentPaths.push_back(path);

            // Reset the size for the next call
            valueNameSize = sizeof(valueName) / sizeof(valueName[0]);
        }
        // Close the registry key
        RegCloseKey(hKey);
    }
    return recentPaths;
}

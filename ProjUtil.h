#ifndef PROJ_UTIL_H
#define PROJ_UTIL_H

#include <vector>
#include <string>

std::wstring trim(const std::wstring& str);

// Helper function to get recent files paths from registry
std::vector<std::wstring> getRecentFilesPaths();
#endif // PROJ_UTIL_H

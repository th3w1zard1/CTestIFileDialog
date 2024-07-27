#include <iostream>
#include <sstream>  // Include the <sstream> header for std::wstringstream
#include "IFileDialog.h"
#include <limits>
#undef max // Undefine the max macro to prevent limits vs windows.h conflicts

// Helper function to trim leading and trailing whitespace from a string
std::wstring trim(const std::wstring& str) {
    auto start = std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    auto end = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base();

    return (start < end) ? std::wstring(start, end) : std::wstring();
}

// Helper function to get user input as a string with default
std::wstring getUserInputStr(const std::wstring& prompt, const std::wstring& defaultVal) {
    std::wcout << prompt << L" (default: " << defaultVal << L"): ";
    std::wstring input;
    std::getline(std::wcin, input);
    if (trim(input).empty()) {
        return defaultVal;
    }
    return input;
}

// Helper function to get user input as a boolean with default
int getUserInputTriBool(const std::wstring& prompt, int defaultVal) {
    if (defaultVal == -1) {
        std::wcout << prompt << L" (default: -1): ";
    } else{
        std::wcout << prompt << L" (default: " << (defaultVal ? L"yes" : L"no") << L"): ";
    }
    std::wstring input;
    std::getline(std::wcin, input);
    input = trim(input);

    // Convert input to lowercase for easier comparison
    for (auto& c : input) {
        c = std::tolower(c);
    }

    // Check for various forms of yes/no input
    if (input == L"yes" || input == L"y" || input == L"true" || input == L"1") {
        return 1;
    } else if (input == L"no" || input == L"n" || input == L"false" || input == L"0") {
        return 0;
    } else if (input.empty()) {
        return defaultVal;
    } else {
        // If input is not recognized, ask again
        std::wcout << L"Invalid input '" + input + L"'. Please enter yes/no, y/n, true/false, or 1/0." << std::endl;
        return getUserInputTriBool(prompt, defaultVal);
    }
}

void showConfigMenu() {
    std::wcout << L"Configure File Dialog Options:\n";
    std::wcout << L"1. Allow multiple selection (default: no)\n";
    std::wcout << L"2. Add to recent (default: yes)\n";
    std::wcout << L"3. Show hidden files (default: no)\n";
    std::wcout << L"4. No change dir (default: no)\n";
    std::wcout << L"5. Confirm overwrite (only for save dialog, default: yes)\n";
    std::wcout << L"6. Hide MRU places (default: no)\n";
    std::wcout << L"7. Hide pinned places (default: no)\n";
    std::wcout << L"8. Share aware (default: no)\n";
    std::wcout << L"Enter your choices as a comma-separated list (e.g., 1,3,5): ";
}

// Helper function to configure file dialog options based on user input
DWORD getFileDialogOptions(bool isSaveDialog) {
    DWORD options = FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST | FOS_FORCEFILESYSTEM;

    showConfigMenu();
    std::wstring input;
    std::getline(std::wcin, input);
    input = trim(input);

    if (!input.empty()) {
        std::wstringstream ss(input);
        std::wstring token;
        while (std::getline(ss, token, L',')) {
            int choice = std::stoi(trim(token));
            switch (choice) {
                case 1:
                    options |= FOS_ALLOWMULTISELECT;
                    break;
                case 2:
                    options &= ~FOS_DONTADDTORECENT;
                    break;
                case 3:
                    options |= FOS_FORCESHOWHIDDEN;
                    break;
                case 4:
                    options |= FOS_NOCHANGEDIR;
                    break;
                case 5:
                    if (isSaveDialog) {
                        options |= FOS_OVERWRITEPROMPT;
                    }
                    break;
                case 6:
                    options |= FOS_HIDEMRUPLACES;
                    break;
                case 7:
                    options |= FOS_HIDEPINNEDPLACES;
                    break;
                case 8:
                    options |= FOS_SHAREAWARE;
                    break;
                default:
                    std::wcout << L"Invalid choice: " << choice << std::endl;
                    break;
            }
        }
    }

    // Handle defaults
    if (!(options & FOS_ALLOWMULTISELECT)) {
        options &= ~FOS_ALLOWMULTISELECT;
    }
    if (!(options & FOS_DONTADDTORECENT)) {
        options |= FOS_DONTADDTORECENT;
    }
    if (!(options & FOS_FORCESHOWHIDDEN)) {
        options &= ~FOS_FORCESHOWHIDDEN;
    }
    if (!(options & FOS_NOCHANGEDIR)) {
        options &= ~FOS_NOCHANGEDIR;
    }
    if (isSaveDialog && !(options & FOS_OVERWRITEPROMPT)) {
        options |= FOS_OVERWRITEPROMPT;
    }
    if (!(options & FOS_HIDEMRUPLACES)) {
        options &= ~FOS_HIDEMRUPLACES;
    }
    if (!(options & FOS_HIDEPINNEDPLACES)) {
        options &= ~FOS_HIDEPINNEDPLACES;
    }
    if (!(options & FOS_SHAREAWARE)) {
        options &= ~FOS_SHAREAWARE;
    }

    return options;
}


// Text-based menu to select options
void showMenu() {
    std::wcout << L"Select Dialog Type:\n";
    std::wcout << L"1. Open File Dialog\n";
    std::wcout << L"2. Save File Dialog\n";
    std::wcout << L"3. Base File Dialog (parent of open/save dialogs)\n";
    std::wcout << L"Choose an option: ";
}

int getUserMenuChoice(int min, int max, bool acceptsDefaultNoInput) {
    int choice;
    while (true) {
        std::wstring input;
        std::getline(std::wcin, input);
        input = trim(input);
        if (input.empty() && acceptsDefaultNoInput) {
            return -1;
        }
        if (input.empty()) {
            std::wcerr << L"Invalid input. Please enter a number." << std::endl;
            showMenu();
            continue;
        }
        std::wstringstream ss(input);
        if (ss >> choice) {
            if (choice >= min && choice <= max) {
                break;
            } else {
                std::wcerr << L"Invalid choice. Please enter a number between 1 and 3." << std::endl;
                showMenu();
            }
        } else {
            std::wcerr << L"Invalid input. Please enter a valid number." << std::endl;
            showMenu();
        }
    }
    return choice;
}

int main() {
    COMFunctionPointers comFuncs = LoadCOMFunctionPointers();

    if (!comFuncs.pCoInitialize || !comFuncs.pCoCreateInstance ||
        !comFuncs.pCoTaskMemFree || !comFuncs.pCoUninitialize || !comFuncs.pSHCreateItemFromParsingName) {
        std::cerr << "Failed to load one or more COM functions." << std::endl;
        return 1;
    }

    HRESULT hr = comFuncs.pCoInitialize(NULL);
    if (FAILED(hr)) {
        std::cerr << "CoInitialize failed: " << hr << std::endl;
        return 1;
    }

    showMenu();
    int choice = getUserMenuChoice(1, 3, true);
    bool isSaveDialog = (choice == 2);

    IFileDialog* pFileDialog = NULL;
    createFileDialog(comFuncs, &pFileDialog, isSaveDialog);

    std::wstring title = getUserInputStr(L"Enter dialog title: ", L"My C++ IFileOpenDialog");
    std::wstring defaultFolder = getUserInputStr(L"Enter default folder path: ", L"C:");

    std::vector<COMDLG_FILTERSPEC> filters;
    int numFilters = getUserInputTriBool(L"Enter number of file filters: ", -1);
    if (numFilters == -1) {
        // User pressed Enter without entering a number, so we'll use the default number of filters (Any, .txt)
        filters.push_back({ L"Any files", L"*.*" });
        filters.push_back({ L"Text files", L"*.txt" });
    } else {
        for (int i = 0; i < numFilters; ++i) {
            std::wstring filterName = getUserInputStr(L"Enter filter name: ", L"required");
            std::wstring filterSpec = getUserInputStr(L"Enter filter spec: ", L"required");
            filters.push_back({ filterName.c_str(), filterSpec.c_str() });
        }
    }

    DWORD options = getFileDialogOptions(isSaveDialog);
    configureFileDialog(comFuncs, pFileDialog, filters, defaultFolder, options);
    showDialog(comFuncs, pFileDialog);

    if (!isSaveDialog) {
        IFileOpenDialog* pFileOpenDialog = static_cast<IFileOpenDialog*>(pFileDialog);
        std::vector<std::wstring> results = getFileDialogResults(comFuncs, pFileOpenDialog);
        for (const auto& filePath : results) {
            std::wcout << L"Selected file: " << filePath << std::endl;
        }
    }

    pFileDialog->Release();
    comFuncs.pCoUninitialize();
    FreeCOMFunctionPointers(comFuncs);

    return 0;
}

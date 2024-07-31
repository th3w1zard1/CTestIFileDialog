#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <stdexcept>
#include "IFileDialog.h"

// Undefine the max macro to prevent limits vs windows.h conflicts
#undef max

enum OptionState { Default = 0, Enabled = 1, Disabled = -1 };

// Function to generate a random number in the range [low, high]
int GetRandomNumber(int low, int high) {
    if (low > high) throw std::invalid_argument("Lower bound must be less than or equal to upper bound.");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(low, high);
    return dis(gen);
}

// Function to get user input as a string with validation
std::wstring getUserInputStr(const std::wstring& prompt, const std::wstring& defaultValue) {
    std::wstring input;
    std::wcout << prompt;
    std::getline(std::wcin, input);

    if (input.empty()) {
        return defaultValue;
    }

    return input;
}

// Function to get user input as an integer with validation
int getUserInputInt(const std::wstring& prompt, const std::vector<int>& validChoices, int defaultValue) {
    std::wcerr << L"DEBUG: Prompt: " << prompt << std::endl;
    std::wcerr << L"DEBUG: Valid Choices: ";
    for (const auto& choice : validChoices) {
        std::wcerr << choice << L" ";
    }
    std::wcerr << std::endl;
    std::wcerr << L"DEBUG: Default Value: " << defaultValue << std::endl;

    while (true) {
        std::wstring input;
        std::wcout << prompt;
        std::getline(std::wcin, input);

        std::wcerr << L"DEBUG: User input: \"" << input << L"\"" << std::endl;

        if (input.empty()) {
            std::wcerr << L"DEBUG: No input provided, using default value: " << defaultValue << std::endl;
            return defaultValue;
        }

        try {
            int value = std::stoi(input);
            std::wcerr << L"DEBUG: Parsed input: " << value << std::endl;

            if (std::find(validChoices.begin(), validChoices.end(), value) != validChoices.end()) {
                std::wcerr << L"DEBUG: Input is a valid choice: " << value << std::endl;
                return value;
            } else {
                std::wcerr << L"DEBUG: Input is not a valid choice: " << value << std::endl;
            }
        } catch (const std::exception& e) {
            std::wcerr << L"DEBUG: Exception caught: " << e.what() << std::endl;
        }

        std::wcout << L"Invalid input. Please enter a valid choice." << std::endl;
    }
}

// Function to manage file type filters
void manageFilters(std::vector<COMDLG_FILTERSPEC>& filters, bool randomize) {
    if (randomize) {
        int numFilters = GetRandomNumber(1, 25);
        filters.push_back({ _wcsdup(L"All Files"), _wcsdup(L"*.*") });
        for (int i = 0; i < numFilters; ++i) {
            std::wstring filterName = L"Random Filter " + std::to_wstring(i + 1);
            std::wstring filterSpec = L"*." + std::to_wstring(i + 1);
            filters.push_back({ _wcsdup(filterName.c_str()), _wcsdup(filterSpec.c_str()) });
        }
        return;
    }

    while (true) {
        std::wcout << L"\nFile Type Filters Menu:\n";
        std::wcout << L"1. Add New Filter\n";
        for (size_t i = 0; i < filters.size(); ++i) {
            std::wcout << (i + 2) << L". Edit Filter: " << filters[i].pszName << L" (" << filters[i].pszSpec << L")\n";
        }
        std::wcout << (filters.size() + 2) << L". Done\n";
        int choice = getUserInputInt(L"Choose an option: ", { 1, static_cast<int>(filters.size() + 2) }, filters.size() + 2);

        if (choice == 1) {
            std::wstring filterName = getUserInputStr(L"Enter filter name: ", L"Default Filter");
            std::wstring filterSpec = getUserInputStr(L"Enter filter spec: ", L"*.*");
            filters.push_back({ _wcsdup(filterName.c_str()), _wcsdup(filterSpec.c_str()) });
        } else if (choice == filters.size() + 2) {
            break;
        } else {
            size_t filterIndex = choice - 2;
            std::wcout << L"\nEditing Filter: " << filters[filterIndex].pszName << L" (" << filters[filterIndex].pszSpec << L")\n";
            filters[filterIndex].pszName = _wcsdup(getUserInputStr(L"Enter new filter name: ", filters[filterIndex].pszName).c_str());
            filters[filterIndex].pszSpec = _wcsdup(getUserInputStr(L"Enter new filter spec: ", filters[filterIndex].pszSpec).c_str());
        }
    }
}

// Function to display and get user choices for file dialog options
void configureDialogOptions(std::vector<int>& optionStates, bool randomize) {
    struct Option {
        std::wstring name;
        int defaultValue;
    };

    std::vector<Option> dialogOptions = {
        { L"Allow multiple selection", Default },
        { L"Do not add to recent", Default },
        { L"Show hidden files", Default },
        { L"No change dir", Default },
        { L"Confirm overwrite", Default },
        { L"Hide MRU places", Default },
        { L"Hide pinned places", Default },
        { L"Share aware", Default }
    };

    optionStates.resize(dialogOptions.size(), Default); // Ensure optionStates is properly initialized

    if (randomize) {
        for (size_t i = 0; i < dialogOptions.size(); ++i) {
            optionStates[i] = GetRandomNumber(-1, 1);
        }
        return;
    }

    auto optionStateToString = [](int state) {
        switch (state) {
            case Enabled: return L"Enabled";
            case Disabled: return L"Disabled";
            default: return L"Default";
        }
    };

    while (true) {
        std::wcout << L"\nDialog Options Menu:\n";
        for (size_t i = 0; i < dialogOptions.size(); ++i) {
            std::wcout << (i + 1) << L". " << dialogOptions[i].name << L" (Current: " << optionStateToString(optionStates[i]) << L")\n";
        }
        std::wcout << (dialogOptions.size() + 1) << L". Done\n";

        std::vector<int> validChoices;
        for (int i = 1; i <= dialogOptions.size() + 1; ++i) {
            validChoices.push_back(i);
        }
        
        int choice = getUserInputInt(L"Choose an option to change or done to continue: ", validChoices, dialogOptions.size() + 1);
        std::wcerr << L"DEBUG: User chose option: " << choice << std::endl;

        if (choice == dialogOptions.size() + 1) {
            break;
        } else {
            size_t optionIndex = choice - 1;
            std::wcerr << L"DEBUG: Changing option: " << dialogOptions[optionIndex].name << std::endl;

            int newValue = getUserInputInt(L"Select " + dialogOptions[optionIndex].name + L" option:\n1. Enabled\n2. Disabled\n3. Default\nChoose an option: ", { 1, 2, 3 }, 3);
            std::wcerr << L"DEBUG: New value for " << dialogOptions[optionIndex].name << L": " << newValue << std::endl;

            optionStates[optionIndex] = (newValue == 1) ? Enabled : (newValue == 2) ? Disabled : Default;
        }
    }
}


// Function to get file dialog options from user
DWORD getFileDialogOptions(bool isSaveDialog, bool randomize, std::vector<COMDLG_FILTERSPEC>& filters) {
    DWORD options = FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST | FOS_FORCEFILESYSTEM;

    std::vector<int> optionStates;
    configureDialogOptions(optionStates, randomize);

    struct OptionFlag {
        DWORD flag;
        bool isSaveDialogOnly;
    };

    std::vector<OptionFlag> optionFlags = {
        { FOS_ALLOWMULTISELECT, false },
        { FOS_DONTADDTORECENT, false },
        { FOS_FORCESHOWHIDDEN, false },
        { FOS_NOCHANGEDIR, false },
        { FOS_OVERWRITEPROMPT, true },
        { FOS_HIDEMRUPLACES, false },
        { FOS_HIDEPINNEDPLACES, false },
        { FOS_SHAREAWARE, false }
    };

    for (size_t i = 0; i < optionStates.size(); ++i) {
        if (optionStates[i] == Enabled) {
            options |= optionFlags[i].flag;
        } else if (optionStates[i] == Disabled) {
            options &= ~optionFlags[i].flag;
        }
    }

    manageFilters(filters, randomize);

    return options;
}

int main() {
    while (true) {
        std::wcout << L"Select Dialog Type:\n1. Open File Dialog\n2. Save File Dialog\n3. Base File Dialog (parent of open/save dialogs)\n4. Randomize all options\nChoose an option: ";
        int dialogType = getUserInputInt(L"", { 1, 2, 3, 4 }, 4);

        bool randomize = (dialogType == 4);
        bool isSaveDialog = (dialogType == 2);

        std::wstring title = randomize ? L"My C++ IFileOpenDialog" : getUserInputStr(L"Dialog title (default: My C++ IFileOpenDialog): ", L"My C++ IFileOpenDialog");
        std::wstring defaultFolder = randomize ? L"C:" : getUserInputStr(L"Default folder path (default: C:): ", L"C:");

        std::vector<COMDLG_FILTERSPEC> filters;
        DWORD options = getFileDialogOptions(isSaveDialog, randomize, filters);

        COMFunctionPointers comFuncs = LoadCOMFunctionPointers();
        try {
            if (!comFuncs.pCoInitialize || !comFuncs.pCoCreateInstance ||
                !comFuncs.pCoTaskMemFree || !comFuncs.pCoUninitialize || !comFuncs.pSHCreateItemFromParsingName) {
                throw std::runtime_error("Failed to load one or more COM functions.");
            }

            HRESULT hr = comFuncs.pCoInitialize(NULL);
            if (FAILED(hr)) {
                throw std::runtime_error("CoInitialize failed: " + std::to_string(hr));
            }

            IFileDialog* pFileDialog = nullptr;
            createFileDialog(comFuncs, &pFileDialog, isSaveDialog);

            if (pFileDialog == nullptr) {
                throw std::runtime_error("Failed to create file dialog.");
            }

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

        } catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
            if (comFuncs.pCoUninitialize) comFuncs.pCoUninitialize();
            FreeCOMFunctionPointers(comFuncs);
            return 1;
        }

        std::wcout << L"Do you want to configure another dialog? (yes/no): ";
        std::wstring continueInput;
        std::getline(std::wcin, continueInput);
        if (continueInput != L"yes") {
            break;
        }
    }

    return 0;
}

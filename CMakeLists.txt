cmake_minimum_required(VERSION 3.18)

project(CTestIFileDialog VERSION 0.1.0 LANGUAGES C CXX)

# Include VCPKG toolchain
if (DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

add_executable(CIFileDialogTester.exe IFileDialog.cpp main.cpp ProjUtil.cpp ProjWinUtils.cpp)

find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(CIFileDialogTester.exe PRIVATE CLI11::CLI11)

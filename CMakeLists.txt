cmake_minimum_required(VERSION 3.15)

project(CIFileDialogTester)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(CIFileDialogTester ${SOURCES})

# Include vcpkg
if(CMAKE_TOOLCHAIN_FILE)
  include(${CMAKE_TOOLCHAIN_FILE})
endif()

# Find and link dependencies
find_package(Advapi32 REQUIRED)

target_link_libraries(CIFileDialogTester PRIVATE Advapi32::Advapi32)

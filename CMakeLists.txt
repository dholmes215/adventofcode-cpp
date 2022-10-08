cmake_minimum_required(VERSION 3.20.3)

# Update submodules.
include(cmake/UpdateSubmodule.cmake)
option(GIT_SUBMODULE "Check submodules during build" ON)
update_submodule("${CMAKE_CURRENT_LIST_DIR}/vcpkg")

# Configure vcpkg (requires vcpkg submodule)
include(cmake/ConfigureVcpkg.cmake)
configure_vcpkg("${CMAKE_CURRENT_LIST_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(aoc VERSION 0.0.1
            DESCRIPTION " David's C++ Solutions to Advent of Code "
            HOMEPAGE_URL https://github.com/dholmes215/adventofcode-cpp
            LANGUAGES C CXX)

# Find dependencies.
# find_package(absl REQUIRED)
find_package(Catch2 REQUIRED)
find_package(ctre CONFIG REQUIRED)
find_package(cxxopts REQUIRED)
find_package(dh-term REQUIRED)
find_package(fmt REQUIRED)
find_package(mdspan REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(range-v3 REQUIRED)
find_package(tl-expected REQUIRED)

# Targets can "link" this "library" to inherit project options.
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20) # Require _at least_ C++20.

# /Zc:__cplusplus is required to make __cplusplus accurate
# /Zc:__cplusplus is available starting with Visual Studio 2017 version 15.7
# (according to https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus)
# That version is equivalent to _MSC_VER==1914
# (according to https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019)
# CMake's ${MSVC_VERSION} is equivalent to _MSC_VER
# (according to https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html#variable:MSVC_VERSION)
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
	target_compile_options(project_options INTERFACE "/Zc:__cplusplus")
endif()

# Targets can "link" this "library" to inherit the warnings specified in CompilerWarnings.cmake.
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# Allow for static analysis options.
include(cmake/StaticAnalyzers.cmake)

add_subdirectory(src)

include(CTest)
include(Catch)

add_subdirectory(tests)

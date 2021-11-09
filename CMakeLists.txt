cmake_minimum_required(VERSION 3.20.3)

# Update submodules.
include(cmake/UpdateSubmodule.cmake)
option(GIT_SUBMODULE "Check submodules during build" ON)
update_submodule("${CMAKE_CURRENT_LIST_DIR}/vcpkg")

# Configure vcpkg (requires vcpkg submodule)
include(cmake/ConfigureVcpkg.cmake)
configure_vcpkg("${CMAKE_CURRENT_LIST_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(hello VERSION 0.0.1
              DESCRIPTION "Application to print \"Hello World!\""
              HOMEPAGE_URL https://github.com/dholmes215/cpp-template-2
              LANGUAGES CXX)

# Find dependencies.
find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)

# Targets can "link" this "library" to inherit project options.
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20) # Require _at least_ C++20.

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

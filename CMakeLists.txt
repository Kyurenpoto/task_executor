cmake_minimum_required (VERSION 3.8)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set (CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif ()

project (task_executor VERSION 0.1.0
         DESCRIPTION "C++ Task Executor inspired by https://github.com/chriskohlhoff/executors"
         HOMEPAGE_URL https://github.com/Kyurenpoto/task_executor
         LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_library (task_executor INTERFACE)
target_include_directories (task_executor INTERFACE include/)
if(MSVC)
    add_compile_options(/std:c++latest /W3 /await)
else()
    add_compile_options(-std=c++2a -Wall -fcoroutines-ts)
endif()

add_subdirectory (tests)

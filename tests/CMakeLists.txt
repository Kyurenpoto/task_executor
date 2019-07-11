cmake_minimum_required (VERSION 3.8)

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set (CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
	    CACHE STRING "")
endif ()

project (task_executor_tests CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing ()

add_executable (task_executor_tests test.cpp)
find_package (gtest REQUIRED)
target_link_libraries (task_executor_tests GTest::GTest GTest::Main)
add_test (task_executor_tests task_executor_tests)

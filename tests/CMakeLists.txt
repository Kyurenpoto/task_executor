cmake_minimum_required (VERSION 3.8)

project (task_executor_tests CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing ()

add_executable (task_executor_tests test.cpp)
target_link_libraries (task_executor_tests PRIVATE task_executor doctest)
add_test (task_executor_tests task_executor_tests)

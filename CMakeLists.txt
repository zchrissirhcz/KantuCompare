cmake_minimum_required(VERSION 3.20)

project(KantuCompare)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(KANTU_USE_PVS "Use PVS-Studio for analysis?" OFF)
option(KANTU_TESTING "Build unit test?" ON)

include("cmake/output_dir.cmake")
include("cmake/deps.cmake")
include("cmake/lldb-debug.cmake")
include("cmake/overlook.cmake")
#include("cmake/asan.cmake")
#include("cmake/tsan.cmake")

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src/kantu)

if(KANTU_TESTING)
  enable_testing()
  add_subdirectory(src/kantu_tests)
endif()

include(cmake/summary.cmake)
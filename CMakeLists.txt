cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory")
endif()

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
include("cmake/ninja_colorful_output.cmake")
#include("cmake/asan.cmake")
#include("cmake/tsan.cmake")

add_subdirectory(src/kantu)

if(KANTU_TESTING)
  enable_testing()
  add_subdirectory(src/kantu_tests)
endif()

include(cmake/summary.cmake)
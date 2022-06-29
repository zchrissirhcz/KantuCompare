cmake_minimum_required(VERSION 3.20)

project(ImageCompare)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(IMCMP_USE_PVS "Use PVS-Studio for analysis?" OFF)
option(IMCMP_TESTING "Build unit test?" ON)

include("cmake/output_dir.cmake")
include("cmake/sleek.cmake")
include("cmake/deps.cmake")
#include("cmake/asan.cmake")
#include("cmake/tsan.cmake")

set(CMAKE_BUILD_TYPE Debug)
sleek_add_flags("-fstandalone-debug")

if(IMCMP_TESTING)
  enable_testing()
endif()

add_subdirectory(src)
add_subdirectory(tests)

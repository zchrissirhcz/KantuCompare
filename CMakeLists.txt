cmake_minimum_required(VERSION 3.20)

project(image_compare)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(sleek.cmake)

set(CMAKE_BUILD_TYPE Debug)
sleek_add_flags("-fstandalone-debug")

# imgui: using the latest source code
set(IMGUI_DIR "$ENV{HOME}/work/github/imgui")
#set(IMGUI_DIR "$ENV{HOME}/work/imgui-docking")
#add_definitions(-DIMGUI_WITH_DOCKING)
include_directories(
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
)

# glfw: using the latest source code
# find_package(glfw3 REQUIRED)
set(GLFW_DIR "$ENV{HOME}/work/github/glfw") # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} ${CMAKE_BINARY_DIR}/glfw EXCLUDE_FROM_ALL)

# opengl: using the system bundled
find_package(OpenGL REQUIRED)

# opencv: using the prebuilt
set(OpenCV_DIR "$ENV{HOME}/artifacts/opencv/4.5.5/mac-arm64/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

add_executable(ImageCompare
  main.cpp
  RichImage.hpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  tinyfiledialogs.h
  tinyfiledialogs.c
  image_compare_core.hpp
  image_compare_core.cpp
  my_widgets.hpp
  my_widgets.cpp
)
target_link_libraries(ImageCompare
  glfw
  ${OPENGL_LIBRARIES}
  ${OpenCV_LIBS}
)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")

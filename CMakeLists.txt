cmake_minimum_required(VERSION 3.20)

project(image_compare)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# imgui
set(IMGUI_DIR "$ENV{HOME}/work/imgui")
include_directories(
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
)

# glfw
# find_package(glfw3 REQUIRED)
set(GLFW_DIR "$ENV{HOME}/work/glfw") # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)

# opengl
find_package(OpenGL REQUIRED)

# opencv
set(OpenCV_DIR "/Users/zz/artifacts/opencv/4.5.5/mac-arm64/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

add_executable(ImageCompare
  main.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  tinyfiledialogs.c
)
target_link_libraries(ImageCompare
  glfw
  ${OPENGL_LIBRARIES}
  ${OpenCV_LIBS}
)

message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")

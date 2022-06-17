cmake_minimum_required(VERSION 3.20)

project(image_compare)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("sleek.cmake")
set(CMAKE_BUILD_TYPE Debug)
sleek_add_flags("-fstandalone-debug")

include("deps.cmake")

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
  image_compare_core.hpp
  image_compare_core.cpp
)
target_link_libraries(ImageCompare
  glfw
  ${OPENGL_LIBRARIES}
  ${OpenCV_LIBS}
  #tinyfiledialogs
  portable_file_dialogs
)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
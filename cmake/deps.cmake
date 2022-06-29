#----------------------------------------------------------------------
# OpenCV
#----------------------------------------------------------------------
# using the prebuilt
#   sudo apt install libopencv-dev # ubuntu
#   brew install opencv # mac
#----------------------------------------------------------------------
#set(OpenCV_DIR "$ENV{HOME}/artifacts/opencv/4.5.5/mac-arm64/lib/cmake/opencv4")
set(OpenCV_DIR "$ENV{HOME}/.sled/artifacts/opencv/4.5.5/lib/cmake/opencv4")
message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")
find_package(OpenCV REQUIRED)


#----------------------------------------------------------------------
# Dear ImGui
#----------------------------------------------------------------------
# git clone https://github.com/ocornut/imgui -b docking imgui-docking
# mirror: https://gitee.com/mirrors/imgui
#----------------------------------------------------------------------
#set(IMGUI_DIR "$ENV{HOME}/work/github/imgui")
# using the docking branch
set(IMGUI_DIR "$ENV{HOME}/.sled/work_repos/imgui/docking")
#set(IMGUI_DIR "$ENV{HOME}/work/imgui-docking")
add_definitions(-DIMGUI_WITH_DOCKING)
include_directories(
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
)


#----------------------------------------------------------------------
# GLFW
#----------------------------------------------------------------------
# using the latest source code
#   git clone https://github.com/glfw/glfw
#   mirror: https://gitee.com/mirrors/glfw
#----------------------------------------------------------------------
# find_package(glfw3 REQUIRED)
#set(GLFW_DIR "$ENV{HOME}/work/github/glfw") # Set this to point to an up-to-date GLFW repo
set(GLFW_DIR "$ENV{HOME}/.sled/work_repos/glfw/master")
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} ${CMAKE_BINARY_DIR}/glfw EXCLUDE_FROM_ALL)


#----------------------------------------------------------------------
# portable-file-dialogs
#----------------------------------------------------------------------
set(portable_file_dialogs_DIR "$ENV{HOME}/.sled/work_repos/portable-file-dialogs/master")
add_subdirectory(${portable_file_dialogs_DIR} ${CMAKE_BINARY_DIR}/portable_file_dialogs)


#----------------------------------------------------------------------
# OpenGL
#----------------------------------------------------------------------
# using the system bundled
#----------------------------------------------------------------------
find_package(OpenGL REQUIRED)


#----------------------------------------------------------------------
# Googletest
#----------------------------------------------------------------------
if(IMCMP_TESTING)
  set(GTest_DIR "$ENV{HOME}/.sled/artifacts/googletest/release-1.11.0/lib/cmake/GTest/")
  message(STATUS "GTest_DIR: ${GTest_DIR}")
  find_package(GTest REQUIRED)
endif()
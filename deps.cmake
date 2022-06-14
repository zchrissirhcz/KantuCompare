#----------------------------------------------------------------------
# OpenCV
#----------------------------------------------------------------------
# using the prebuilt
#   sudo apt install libopencv-dev # ubuntu
#   brew install opencv # mac
#----------------------------------------------------------------------
set(OpenCV_DIR "$ENV{HOME}/artifacts/opencv/4.5.5/mac-arm64/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)


#----------------------------------------------------------------------
# Dear ImGui
#----------------------------------------------------------------------
# git clone https://github.com/ocornut/imgui -b docking imgui-docking
# mirror: https://gitee.com/mirrors/imgui
#----------------------------------------------------------------------
#set(IMGUI_DIR "$ENV{HOME}/work/github/imgui")
# using the docking branch
set(IMGUI_DIR "$ENV{HOME}/work/github/imgui-docking")
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
set(GLFW_DIR "$ENV{HOME}/work/github/glfw") # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} ${CMAKE_BINARY_DIR}/glfw EXCLUDE_FROM_ALL)


#----------------------------------------------------------------------
# OpenGL
#----------------------------------------------------------------------
# using the system bundled
#----------------------------------------------------------------------
find_package(OpenGL REQUIRED)


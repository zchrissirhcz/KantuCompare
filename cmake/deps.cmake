#----------------------------------------------------------------------
# OpenCV
#----------------------------------------------------------------------
# using the prebuilt
#   sudo apt install libopencv-dev # ubuntu
#   brew install opencv # mac
#----------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/deps/opencv/install")
  if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/deps/opencv/install/x64/vc17/staticlib")
  else()
    set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/deps/opencv/install/x64/staticlib")
  endif()
else()
  #set(OpenCV_DIR "D:/artifacts/opencv/windows/OpenCV-4.6.0-vs2015-x64-static")
endif()
message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")
find_package(OpenCV REQUIRED)


#----------------------------------------------------------------------
# Dear ImGui
#----------------------------------------------------------------------
# git clone https://github.com/ocornut/imgui -b docking imgui-docking
# mirror: https://gitee.com/mirrors/imgui
#----------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/deps/imgui")
  set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/deps/imgui")
else()
  #set(IMGUI_DIR "$ENV{HOME}/work/github/imgui")
  #set(IMGUI_DIR "$ENV{HOME}/.sled/work_repos/imgui/docking")
  #set(IMGUI_DIR "$ENV{HOME}/work/imgui-docking")
endif()
#add_definitions(-DIMGUI_WITH_DOCKING)
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
if(EXISTS "${CMAKE_SOURCE_DIR}/deps/glfw")
  set(GLFW_DIR "${CMAKE_SOURCE_DIR}/deps/glfw")
else()
  #set(GLFW_DIR "$ENV{HOME}/.sled/work_repos/glfw/master")
endif()
# find_package(glfw3 REQUIRED)
#set(GLFW_DIR "$ENV{HOME}/work/github/glfw") # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} ${CMAKE_BINARY_DIR}/glfw EXCLUDE_FROM_ALL)


#----------------------------------------------------------------------
# portable-file-dialogs
#----------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/deps/portable-file-dialogs")
  set(portable_file_dialogs_DIR "${CMAKE_SOURCE_DIR}/deps/portable-file-dialogs")
else()
  #set(portable_file_dialogs_DIR "$ENV{HOME}/.sled/work_repos/portable-file-dialogs/master")
endif()
add_subdirectory(${portable_file_dialogs_DIR} ${CMAKE_BINARY_DIR}/portable_file_dialogs)


#----------------------------------------------------------------------
# OpenGL
#----------------------------------------------------------------------
# using the system bundled
#----------------------------------------------------------------------
find_package(OpenGL REQUIRED)
message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")


#----------------------------------------------------------------------
# Str
#----------------------------------------------------------------------
add_library(str INTERFACE
  ${CMAKE_SOURCE_DIR}/deps/str/Str.h
)
target_include_directories(str INTERFACE ${CMAKE_SOURCE_DIR}/deps)


#----------------------------------------------------------------------
# Googletest
#----------------------------------------------------------------------
if(KANTU_TESTING)
  if(EXISTS "${CMAKE_SOURCE_DIR}/deps/googletest")
    set(GTest_DIR "${CMAKE_SOURCE_DIR}/deps/googletest/install/lib/cmake/GTest")
  else()
    #set(GTest_DIR "$ENV{HOME}/.sled/artifacts/googletest/release-1.11.0/lib/cmake/GTest/")
  endif()
  message(STATUS "GTest_DIR: ${GTest_DIR}")
  find_package(GTest REQUIRED)
endif()


#----------------------------------------------------------------------
# MLCC
#----------------------------------------------------------------------
add_library(mlcc INTERFACE
  ${CMAKE_SOURCE_DIR}/deps/mlcc/fmt1.h
)
target_include_directories(mlcc INTERFACE ${CMAKE_SOURCE_DIR}/deps)
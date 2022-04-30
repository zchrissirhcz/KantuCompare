cmake_minimum_required(VERSION 3.15)

project(ImageCompare VERSION 0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(overlook.cmake)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(OpenCV_DIR "D:/lib/opencv/4.5.5/build")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(OpenCV_DIR "/home/zz/artifacts/opencv/linux-x64/4.5.5/lib/cmake/opencv4")
endif()
find_package(OpenCV REQUIRED)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(Qt5_DIR "D:/soft/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake/Qt5")
  set(QT_DIR "D:/soft/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake/Qt5")
endif()
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)


add_library(image_compare_core STATIC
  image_compare_core.hpp
  image_compare_core.cpp
)
target_link_libraries(image_compare_core PUBLIC ${OpenCV_LIBS})



set(PROJECT_SOURCES
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(ImageCompare
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
  )
else() 
  add_executable(ImageCompare
    ${PROJECT_SOURCES}
  )
endif()

target_link_libraries(ImageCompare PRIVATE Qt${QT_VERSION_MAJOR}::Widgets image_compare_core)

set_target_properties(ImageCompare PROPERTIES
  MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE TRUE
  WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(ImageCompare)
endif()


add_executable(testbed
  testbed.cpp
)
target_link_libraries(testbed image_compare_core)

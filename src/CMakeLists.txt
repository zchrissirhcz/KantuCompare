add_library(image_io STATIC
  ${CMAKE_SOURCE_DIR}/src/image_io.hpp
  ${CMAKE_SOURCE_DIR}/src/image_io.cpp
)
target_include_directories(image_io PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(image_io PUBLIC ${OpenCV_LIBS})

add_library(image_compare STATIC
  ${CMAKE_SOURCE_DIR}/src/image_compare.hpp
  ${CMAKE_SOURCE_DIR}/src/image_compare.cpp
)
target_include_directories(image_compare PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(image_compare PUBLIC ${OpenCV_LIBS})

add_executable(ImageCompare
  ${CMAKE_SOURCE_DIR}/src/app.cpp
  ${CMAKE_SOURCE_DIR}/src/image_render.hpp
  ${CMAKE_SOURCE_DIR}/src/image_render.cpp

  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
)
target_link_libraries(ImageCompare
  glfw
  ${OPENGL_LIBRARIES}
  image_compare
  image_io
  ${OpenCV_LIBS}
  portable_file_dialogs
)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")

if(IMCMP_USE_PVS)
  include(cmake/PVS-Studio.cmake)
  pvs_studio_add_target(TARGET example2.analyze ALL
                        OUTPUT FORMAT errorfile
                        RECURSIVE ANALYZE ImageCompare
                        MODE GA:1,2
                        LOG target.err)
endif()

add_executable(image_viewer
  ${CMAKE_SOURCE_DIR}/src/image_viewer.cpp
)
target_link_libraries(image_viewer image_io)
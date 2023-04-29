# image file loading, format transform, string utilities
add_library(kantu_basic STATIC
  transform_format.hpp
  transform_format.cpp
  image_io.hpp
  image_io.cpp
  string.hpp
  string.cpp
)
target_include_directories(kantu_basic PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(kantu_basic PUBLIC ${OpenCV_LIBS} str)

# compare RGB/Gray images
add_library(kantu_compare STATIC
  compare.hpp
  compare.cpp
)
target_link_libraries(kantu_compare PUBLIC kantu_basic)

# render result and show
add_executable(KantuCompareApp
  app.cpp
  image_render.hpp
  image_render.cpp

  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
)
target_link_libraries(KantuCompareApp
  glfw
  ${OPENGL_LIBRARIES}
  kantu_compare
  portable_file_dialogs
)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

if(KANTU_USE_PVS)
  include(cmake/PVS-Studio.cmake)
  pvs_studio_add_target(TARGET example2.analyze ALL
    OUTPUT FORMAT errorfile
    RECURSIVE ANALYZE KantuCompare
    MODE GA:1,2
    LOG target.err
  )
endif()

# fast debugging with cv::imshow
add_executable(kantu_image_viewer
  image_viewer.cpp
)
target_link_libraries(kantu_image_viewer kantu_basic)
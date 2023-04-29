add_library(kantu_image_format STATIC
  image_format.hpp
  image_format.cpp
)
target_include_directories(kantu_image_format PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(kantu_image_format PUBLIC ${OpenCV_LIBS})

add_library(kantu_image_io STATIC
  image_io.hpp
  image_io.cpp
)
target_include_directories(kantu_image_io PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(kantu_image_io PUBLIC kantu_image_format str)

add_library(kantu_image_compare STATIC
  image_compare.hpp
  image_compare.cpp
)
target_include_directories(kantu_image_compare PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(kantu_image_compare PUBLIC ${OpenCV_LIBS})

add_executable(KantuCompare
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
target_link_libraries(KantuCompare
  glfw
  ${OPENGL_LIBRARIES}
  kantu_image_compare
  kantu_image_io
  ${OpenCV_LIBS}
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

add_executable(kantu_image_viewer
  image_viewer.cpp
)
target_link_libraries(kantu_image_viewer kantu_image_io)
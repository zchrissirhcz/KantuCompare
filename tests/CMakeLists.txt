if(IMCMP_TESTING)
  macro(imcmp_add_test name)
  add_executable(test_${name} ${CMAKE_CURRENT_SOURCE_DIR}/test_${name}.cpp)
  set(dep_libs GTest::gtest GTest::gtest_main)
  if(ANDROID)
    list(APPEND dep_libs log)
  endif()
  target_link_libraries(test_${name} PUBLIC
    ${dep_libs}
    ${ARGN}
  )
  gtest_add_tests(TARGET test_${name})
  endmacro()

  execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/images ${CMAKE_BINARY_DIR}
  )

  imcmp_add_test(image_compare image_compare image_io)
endif()
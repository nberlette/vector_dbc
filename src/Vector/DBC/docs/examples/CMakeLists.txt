include_directories(${PROJECT_SOURCE_DIR}/src)

if(OPTION_RUN_EXAMPLES)

  add_definitions(
    -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

  set(CMAKE_STATIC_LIBRARY_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
  set(CMAKE_SHARED_LIBRARY_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")

  if(WIN32)
    add_custom_target(Vector_DLL-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${PROJECT_BINARY_DIR}/src/Vector/DBC/${CMAKE_BUILD_TYPE}/Vector_DBC.dll"
      "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
      COMMENT "Copy ${PROJECT_BINARY_DIR}/src/Vector/DBC/${CMAKE_BUILD_TYPE}/Vector_DBC.dll to ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
  endif()

  add_executable(example_Messages_Signals Messages_Signals.cpp)

  target_link_libraries(example_Messages_Signals Vector_DBC)

endif()

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/Messages_Signals.cpp
  DESTINATION share/doc/Vector_DBC/examples)

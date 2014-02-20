include_directories(${PROJECT_SOURCE_DIR}/src)

if(OPTION_RUN_EXAMPLES)

  add_definitions(
    -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

  add_executable(example_Messages_Signals Messages_Signals.cpp)

  target_link_libraries(example_Messages_Signals Vector_DBC)

endif()

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/Messages_Signals.cpp
  DESTINATION share/doc/Vector_DBC/examples)

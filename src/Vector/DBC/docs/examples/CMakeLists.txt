include_directories(${PROJECT_SOURCE_DIR}/src)

if(OPTION_USE_CPP11_REGEX)
  find_package(Boost 1.54 COMPONENTS regex REQUIRED)
  include_directories(${Boost_INCLUDE_DIR})
endif()

add_definitions(
  -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
  -DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

add_executable(example_Messages_Signals Messages_Signals.cpp)

target_link_libraries(example_Messages_Signals Vector_DBC ${Boost_LIBRARIES})

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/Messages_Signals.cpp
  DESTINATION share/doc/Vector_DBC/examples)

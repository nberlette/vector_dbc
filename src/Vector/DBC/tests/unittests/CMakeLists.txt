include_directories(${PROJECT_SOURCE_DIR}/src)

find_package(Boost 1.54 COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_definitions(
  -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
  -DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

add_executable(test_File test_File.cpp)
add_executable(test_Message test_Message.cpp)
add_executable(test_Signal test_Signal.cpp)

if(OPTION_USE_GCOV)
  add_definitions(-g -O0 -fprofile-arcs -ftest-coverage)
  target_link_libraries(test_File cov)
  target_link_libraries(test_Message cov)
  target_link_libraries(test_Signal gcov)
endif()

target_link_libraries(test_File
  Vector_DBC
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test_Message
  Vector_DBC
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test_Signal
  Vector_DBC
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

enable_testing()

add_test(File test_File)
add_test(Message test_Message)
add_test(Signal test_Signal)

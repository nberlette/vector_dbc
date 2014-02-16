include_directories(${PROJECT_SOURCE_DIR}/src)

if(OPTION_USE_CPP11_REGEX)
  find_package(Boost 1.54 COMPONENTS system filesystem unit_test_framework REQUIRED)
else()
  find_package(Boost 1.54 COMPONENTS regex system filesystem unit_test_framework REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIR})

add_definitions(
  -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
  -DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

add_executable(test_Database test_Database.cpp)
add_executable(test_Signal test_Signal.cpp)

if(OPTION_USE_GCOV)
  add_definitions(-g -O0 -fprofile-arcs -ftest-coverage)
  target_link_libraries(test_Database Vector_DBC ${Boost_LIBRARIES} gcov)
  target_link_libraries(test_Signal Vector_DBC ${Boost_LIBRARIES} gcov)
else()
  target_link_libraries(test_Database Vector_DBC ${Boost_LIBRARIES})
  target_link_libraries(test_Signal Vector_DBC ${Boost_LIBRARIES})
endif()

enable_testing()

add_test(Database test_Database)
add_test(Signal test_Signal)

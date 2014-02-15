include_directories(${PROJECT_SOURCE_DIR}/src)

if(NOT OPTION_USE_CPP11_REGEX)
  find_package(Boost 1.54 COMPONENTS regex system filesystem unit_test_framework REQUIRED)
else()
  find_package(Boost 1.54 COMPONENTS regex system filesystem REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIR})

add_definitions(
  -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
  -DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

add_executable(test_Database test_Database.cpp)

target_link_libraries(test_Database Vector_DBC ${Boost_LIBRARIES})

enable_testing()

add_test(Database test_Database)

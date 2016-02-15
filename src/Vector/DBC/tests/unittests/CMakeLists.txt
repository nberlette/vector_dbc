include_directories(${PROJECT_SOURCE_DIR}/src)

find_package(Boost 1.55 COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_definitions(
    -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

add_executable(test_File test_File.cpp)
add_executable(test_Message test_Message.cpp)
add_executable(test_Signal test_Signal.cpp)
add_executable(test_Regression test_Regression.cpp)

if(OPTION_USE_GCOV_LCOV)
    add_definitions(-g -O0 -fprofile-arcs -ftest-coverage)
    target_link_libraries(test_File gcov)
    target_link_libraries(test_Message gcov)
    target_link_libraries(test_Signal gcov)
    target_link_libraries(test_Regression gcov)
endif()

target_link_libraries(test_File
    ${PROJECT_NAME}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test_Message
    ${PROJECT_NAME}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test_Signal
    ${PROJECT_NAME}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(test_Regression
    ${PROJECT_NAME}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

enable_testing()

add_test(File test_File)
add_test(Message test_Message)
add_test(Signal test_Signal)
add_test(Regression test_Regression)

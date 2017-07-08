# search paths
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIR})
link_directories(
    ${Boost_LIBRARY_DIRS})

# compiler/linker settings
add_definitions(
    -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

# targets
add_executable(test_File test_File.cpp)
add_executable(test_Message test_Message.cpp)
add_executable(test_Signal test_Signal.cpp)
add_executable(test_Regression test_Regression.cpp)
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

# tests
include(CTest)
enable_testing()
add_test(File test_File)
add_test(Message test_Message)
add_test(Signal test_Signal)
add_test(Regression test_Regression)

# coverage
if(OPTION_USE_GCOV_LCOV)
    target_link_libraries(test_File gcov)
    target_link_libraries(test_Message gcov)
    target_link_libraries(test_Signal gcov)
    target_link_libraries(test_Regression gcov)

    add_custom_target(lcov_capture ALL
        COMMAND ${LCOV_EXECUTABLE}
            --capture
            --directory ${PROJECT_BINARY_DIR}
            --output-file coverage.info || true
        DEPENDS
            test_File
            test_Message
            test_Signal
            test_Regression)

    add_custom_target(lcov_genhtml ALL
        COMMAND ${LCOV_GENHTML_EXECUTABLE}
            coverage.info
            --output-directory lcov || true
        DEPENDS lcov_capture)

    install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lcov
        DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif()

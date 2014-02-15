cmake_minimum_required(VERSION 2.8)

project(Vector_DBC)

option(OPTION_RUN_DOXYGEN "Run Doxygen" ON)
option(OPTION_RUN_CCCC "Run CCCC" ON)
option(OPTION_RUN_CPPCHECK "Run Cppcheck" ON)
option(OPTION_USE_CPP11_REGEX "Use std::regex instead of boost::regex" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
set(CMAKE_INSTALL_PREFIX /)

add_subdirectory(src)

include(CTest)
enable_testing()

include(CPack)

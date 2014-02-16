cmake_minimum_required(VERSION 2.8)

project(Vector_DBC)

# build types: None, Debug, Release, RelWithDebInfo, MinSizeRel
set(CMAKE_BUILD_TYPE Release)

# regex support by C++11 or Boost
option(OPTION_USE_CPP11_REGEX "Use std::regex instead of boost::regex" OFF)

# source code documentation
option(OPTION_RUN_DOXYGEN "Run Doxygen" ON)

# static code analysis
option(OPTION_RUN_CCCC "Run CCCC" OFF)
option(OPTION_RUN_CPPCHECK "Run Cppcheck" OFF)

# coverage analysis
option(OPTION_USE_GCOV_LCOV "Use gcov and run lcov" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
set(CMAKE_INSTALL_PREFIX /)

include(CTest)
include(CPack)

add_subdirectory(src)

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md
  DESTINATION share/doc/Vector_DBC)

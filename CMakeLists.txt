cmake_minimum_required(VERSION 2.8)

project(Vector_DBC)

# build types: None, Debug, Release, RelWithDebInfo, MinSizeRel
set(CMAKE_BUILD_TYPE Release)

# strict names
option(OPTION_USE_STRICT_NAMES "Names normally must be C-identifiers" ON)

# regex support by C++11 or Boost
option(OPTION_USE_CPP11_REGEX "Use std::regex instead of boost::regex" ON)

# use enum class instead of enum
option(OPTION_USE_CPP11_ENUM_CLASS "Use enum class instead of enum" OFF)

# source code documentation
option(OPTION_RUN_DOXYGEN "Run Doxygen" ON)

# static code analysis
option(OPTION_RUN_CCCC "Run CCCC" OFF)
option(OPTION_RUN_CPPCHECK "Run Cppcheck" OFF)

# dynamic tests
option(OPTION_RUN_EXAMPLES "Run examples" OFF)
option(OPTION_RUN_TESTS "Run tests" OFF)
option(OPTION_USE_GCOV_LCOV "Use gcov and run lcov" OFF)

# package
set(CPACK_PACKAGE_CONTACT "Tobias Lorenz <tobias.lorenz@gmx.net>")
set(CPACK_PACKAGE_VENDOR "Tobias Lorenz <tobias.lorenz@gmx.net>")
set(CPACK_PACKAGE_VERSION 1.0.0)
set(CPACK_VERSION_MAJOR 1)
set(CPACK_VERSION_MINOR 1)
set(CPACK_VERSION_PATCH 1)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.GPL)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_NSIS_CONTACT "Tobias Lorenz <tobias.lorenz@gmx.net>")
set(CPACK_NSIS_MUI_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
set(CPACK_NSIS_MUI_UNICON ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
set(CMAKE_INSTALL_PREFIX /)

# Boost (needed for dynamic tests)
if(WIN32)
  set(BOOST_ROOT c:/local/boost_1_54_0)
  set(BOOST_INCLUDEDIR ${BOOST_ROOT})
  set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib32-msvc-11.0)
  set(Boost_USE_STATIC_LIBS ON) # only find static libs
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
endif()

include(CTest)
include(CPack)

add_subdirectory(src)

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.GPL
  DESTINATION share/doc/Vector_DBC)

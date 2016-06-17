cmake_minimum_required(VERSION 2.8)

project(Vector_DBC)

# build types: None, Debug, Release, RelWithDebInfo, MinSizeRel
set(CMAKE_BUILD_TYPE Release)

# strict names
option(OPTION_USE_STRICT_NAMES "Names normally must be C-identifiers" ON)

# regex support by C++11 or Boost
option(OPTION_USE_CPP11_REGEX "Use std::regex instead of boost::regex" ON)

# source code documentation
option(OPTION_RUN_DOXYGEN "Run Doxygen" ON)

# static code analysis
option(OPTION_RUN_CCCC "Run CCCC" OFF)
option(OPTION_RUN_CPPCHECK "Run Cppcheck" OFF)

# dynamic tests
option(OPTION_RUN_EXAMPLES "Run examples" OFF)
option(OPTION_RUN_TESTS "Run tests" OFF)
option(OPTION_USE_GCOV_LCOV "Use gcov and run lcov" OFF)

# directories
include(GNUInstallDirs)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
set(CMAKE_INSTALL_PREFIX /usr)

# package
set(CPACK_PACKAGE_CONTACT "Tobias Lorenz <tobias.lorenz@gmx.net>")
set(CPACK_PACKAGE_VENDOR "Tobias Lorenz <tobias.lorenz@gmx.net>")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 1)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ library to read and write Vector CAN databases")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.GPL)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_CONTACT "Tobias Lorenz <tobias.lorenz@gmx.net>")
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM;TGZ") # 7Z;DEB;IFW;NSIS;NSIS64;RPM;STGZ;TBZ2;TGZ;TXZ;TZ;ZIP
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tobias Lorenz <tobias.lorenz@gmx.net>")
endif()

include(CTest)
include(CPack)

add_subdirectory(src)

install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.md
        ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.GPL
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR})

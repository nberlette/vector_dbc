# dependencies
include(GenerateExportHeader)

# search paths
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR})
if(NOT OPTION_USE_CPP11_REGEX)
    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

# sources/headers
set(source_files
    Attribute.cpp
    AttributeDefinition.cpp
    AttributeRelation.cpp
    BitTiming.cpp
    EnvironmentVariable.cpp
    ExtendedMultiplexor.cpp
    File.cpp
    FileLoad.cpp
    FileSave.cpp
    Message.cpp
    Network.cpp
    Node.cpp
    Signal.cpp
    SignalGroup.cpp
    SignalType.cpp
    ValueTable.cpp)
set(public_header_files
    Attribute.h
    AttributeDefinition.h
    AttributeRelation.h
    AttributeValueType.h
    BitTiming.h
    ByteOrder.h
    EnvironmentVariable.h
    ExtendedMultiplexor.h
    File.h
    Message.h
    Network.h
    Node.h
    platform.h
    Signal.h
    SignalGroup.h
    SignalType.h
    Status.h
    ValueDescriptions.h
    ValueTable.h
    ValueType.h)
set(private_header_files
    )

# generated files
configure_file(config.h.in config.h)
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)

# compiler/linker settings
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
    add_definitions(-D_FORTIFY_SOURCE=2)
    #add_definitions(-fPIE -pie)
    add_definitions(-Wl,-z,relro,-z,now)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
        add_definitions(-fstack-protector-strong)
    endif()
    if(OPTION_USE_GCOV_LCOV)
        add_definitions(-g -O0 -fprofile-arcs -ftest-coverage)
    endif()
endif()

# targets
add_library(${PROJECT_NAME} SHARED
    ${source_files} ${public_header_files} ${private_header_files})
generate_export_header(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN 1)
if(NOT OPTION_USE_CPP11_REGEX)
    target_link_libraries(${PROJECT_NAME} ${Boost_REGEX_LIBRARY})
endif()

# install
install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/config.h
        ${CMAKE_CURRENT_BINARY_DIR}/vector_dbc_export.h
        ${public_header_files}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Vector/DBC)

# sub directories
add_subdirectory(docs)
add_subdirectory(tests)

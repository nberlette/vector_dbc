cmake_minimum_required(VERSION 2.8)

include(GenerateExportHeader)

configure_file(
  config.h.in
  config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

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

set(header_files
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

add_compiler_export_flags()

add_library(${PROJECT_NAME} SHARED ${source_files} ${header_files})

if(OPTION_USE_GCOV_LCOV)
  target_link_libraries(${PROJECT_NAME} gcov)
  add_definitions(-g -O0 -fprofile-arcs -ftest-coverage)
endif()

generate_export_header(${PROJECT_NAME})

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-std=c++11)
endif()

if(NOT OPTION_USE_CPP11_REGEX)
  find_package(Boost 1.55 COMPONENTS regex)
  include_directories(${Boost_INCLUDE_DIR})
  link_directories(${Boost_LIBRARY_DIRS})
  target_link_libraries(${PROJECT_NAME} ${Boost_REGEX_LIBRARY})
endif()

install(
  TARGETS ${PROJECT_NAME}
  DESTINATION lib)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vector_dbc_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    ${header_files}
  DESTINATION include/Vector/DBC)

add_subdirectory(docs)

add_subdirectory(tests)

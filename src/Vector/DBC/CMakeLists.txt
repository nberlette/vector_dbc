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
  Database.cpp
  DatabaseLoad.cpp
  DatabaseSave.cpp
  EnvironmentVariable.cpp
  ExtendedMultiplexor.cpp
  Message.cpp
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
  Database.h
  EnvironmentVariable.h
  ExtendedMultiplexor.h
  Message.h
  Node.h
  Signal.h
  SignalGroup.h
  SignalType.h
  Status.h
  ValueDescriptions.h
  ValueTable.h
  ValueType.h)

add_compiler_export_flags()

add_library(Vector_DBC SHARED ${source_files} ${header_files})

if(OPTION_USE_GCOV_LCOV)
  target_link_libraries(Vector_DBC gcov)
  add_definitions(-g -O0 -fprofile-arcs -ftest-coverage)
endif()

generate_export_header(Vector_DBC)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-std=c++11)
endif()

if(OPTION_USE_CPP11_REGEX)
  add_definitions(-DUSE_CPP11_REGEX)
else()
  find_package(Boost 1.54 COMPONENTS regex)
  include_directories(${Boost_INCLUDE_DIR})
  link_directories(${Boost_LIBRARY_DIRS})
  target_link_libraries(Vector_DBC ${Boost_REGEX_LIBRARY})
endif()

install(
  TARGETS Vector_DBC
  DESTINATION lib)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/vector_dbc_export.h
  DESTINATION include/Vector/DBC)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h ${header_files}
  DESTINATION include/Vector/DBC)

add_subdirectory(docs)

add_subdirectory(tests)

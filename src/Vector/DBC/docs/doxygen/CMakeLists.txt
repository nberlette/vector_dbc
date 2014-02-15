FIND_PACKAGE(Doxygen)

if(DOXYGEN_FOUND)

  configure_file(Doxyfile.in
    Doxyfile @ONLY IMMEDIATE)

  add_custom_target(doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile > doxygen-out.txt 2> doxygen-err.txt
    SOURCES Doxyfile)

  install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/
    DESTINATION share/doc/Vector_DBC/doxygen)

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/doxygen-out.txt ${CMAKE_CURRENT_BINARY_DIR}/doxygen-err.txt
    DESTINATION share/doc/Vector_DBC/doxygen)

  install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man/
    DESTINATION share/man)

endif()

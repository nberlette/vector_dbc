FIND_PACKAGE(Doxygen)

if(DOXYGEN_FOUND)

  if(DOXYGEN_DOT_FOUND)
    set(DOXYGEN_HAVE_DOT YES)
  else()
    set(DOXYGEN_HAVE_DOT NO)
    if(WIN32)
      find_path(DOXYGEN_DOT_PATH
        NAMES dot.exe
        PATHS "C:/Program Files/Graphviz/bin")
      if(EXISTS "${DOXYGEN_DOT_PATH}")
        set(DOXYGEN_HAVE_DOT YES)
        find_program(DOXYGEN_DOT_EXECUTABLE
          NAMES dot.exe
          PATHS "${DOXYGEN_DOT_PATH}")
      endif()
    endif()
  endif()

  if(WIN32)
    find_program(DOXYGEN_HHC_LOCATION
      NAMES hhc.exe
      PATHS "C:/Program Files/HTML Help Workshop")

    set(DOXYGEN_GENERATE_HTMLHELP YES)
    set(DOXYGEN_GENERATE_TREEVIEW NO)
    set(DOXYGEN_GENERATE_MAN NO)
  else()
    set(DOXYGEN_GENERATE_HTMLHELP NO)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_GENERATE_MAN YES)
  endif()

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

  if(NOT WIN32)
    install(
      DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man/
      DESTINATION share/man)
  endif()

endif()

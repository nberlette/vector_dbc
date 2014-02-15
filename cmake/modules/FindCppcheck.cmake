include(FindPackageHandleStandardArgs)

find_program(CPPCHECK_EXECUTABLE cppcheck)

find_package_handle_standard_args(Cppcheck DEFAULT_MSG CPPCHECK_EXECUTABLE)

mark_as_advanced(CPPCHECK_EXECUTABLE)

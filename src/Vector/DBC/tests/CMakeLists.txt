if(OPTION_RUN_CCCC)
  add_subdirectory(cccc)
endif()

if(OPTION_RUN_CPPCHECK)
  add_subdirectory(cppcheck)
endif()

add_subdirectory(unittests)

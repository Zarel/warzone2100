# NOTES:
#  1- Assumes working dir is po dir
#  2- warzone2100 is DOMAIN hardcoded
# INPUTS: MAKE_MO_CMD
if (EXISTS warzone2100.pot AND MAKE_MO_CMD)
  if (CMAKE_HOST_WIN32 AND NOT CYGWIN)
    set (UNIX_WINDOWS_COMMAND WINDOWS_COMMAND)
  else ()
    set (UNIX_WINDOWS_COMMAND UNIX_COMMAND)
  endif ()
  separate_arguments (the_cmd ${UNIX_WINDOWS_COMMAND} "${MAKE_MO_CMD}")
  execute_process (
    COMMAND ${the_cmd}
  )
  execute_process (
    COMMAND ${CMAKE_COMMAND} -E echo timestamp
    OUTPUT_FILE stamp-po
  )
endif ()

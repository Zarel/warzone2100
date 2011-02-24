# NOTES:
#  1- Assumes working dir is top binary dir
#  2- warzone2100 is DOMAIN hardcoded
if (EXISTS warzone2100.po)
  if (EXISTS po/warzone2100.pot)
    execute_process (
      COMMAND sed -f ${REMOVE_POTCDATE_SED} po/warzone2100.pot
      OUTPUT_FILE warzone2100.1po
    )
    execute_process (
      COMMAND sed -f ${REMOVE_POTCDATE_SED} warzone2100.po
      OUTPUT_FILE warzone2100.2po
    )
    execute_process (
      COMMAND ${CMAKE_COMMAND} -E compare_files warzone2100.1po warzone2100.2po
      RESULT_VARIABLE CHANGED
      OUTPUT_QUIET
      ERROR_QUIET
    )
    if (CHANGED)
      file (REMOVE warzone2100.1po warzone2100.2po po/warzone2100.pot)
      file (RENAME warzone2100.po po/warzone2100.pot)
    else ()
      file (REMOVE warzone2100.1po warzone2100.2po warzone2100.po)
    endif ()
  else ()
      file (RENAME warzone2100.po po/warzone2100.pot)
  endif ()
endif ()

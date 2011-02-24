# INPUTS: lang
# Basically renames ${lang}.new.po to ${lang}.po if different, otherwise deletes ${lang}.new.po
if (EXISTS ${lang}.po)
  execute_process (
    COMMAND ${CMAKE_COMMAND} -E compare_files ${lang}.new.po ${lang}.po
    RESULT_VARIABLE CHANGED
    OUTPUT_QUIET
    ERROR_QUIET
  )
  if (CHANGED)
    file (RENAME ${lang}.new.po ${lang}.po)
  else ()
    file (REMOVE ${lang}.new.po)
  endif ()
else ()
  file (RENAME ${lang}.new.po ${lang}.po) # should only happen when generating the po files for the binary dir, when binary dir != source dir
endif ()

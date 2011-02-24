# Available languages.
#first read in the space separated list
file (STRINGS LINGUAS LANGUAGES)
#convert all newlines into ; to have a nice "set"
separate_arguments (LANGUAGES)

# TODO Check if LINGUAS/LANGUAGES is empty?

############ Validation. ###########
# TODO Put some of this stuff in the FindGettext.cmake module
if (NOT GETTEXT_MSGFMT_VERSION)
  execute_process (
    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --version
    OUTPUT_VARIABLE GETTEXT_MSGFMT_VERSION
  )
  string (REGEX MATCH GNU GETTEXT_MSGFMT_GNU ${GETTEXT_MSGFMT_VERSION})
endif ()

if (GETTEXT_MSGFMT_GNU)
  set (MSGFMT_EXTENSION gmo)
  set (MSGFMT_OPTIONS ${MSGFMT_OPTIONS} --statistics)
else ()
  set (MSGFMT_EXTENSION mo)
endif ()

set (MSGMERGE_OPTIONS --no-wrap --width=1 --quiet -s)

find_program (GETTEXT_MSGINIT_EXECUTABLE msginit)
if (NOT GETTEXT_MSGINIT_EXECUTABLE)
  message (FATAL_ERROR "msginit not found")
endif (NOT GETTEXT_MSGINIT_EXECUTABLE)

find_program (GETTEXT_XGETTEXT_EXECUTABLE xgettext)
if (NOT GETTEXT_XGETTEXT_EXECUTABLE )
  message (FATAL_ERROR "xgettext not found")
endif (NOT GETTEXT_XGETTEXT_EXECUTABLE )

find_program (GETTEXT_MSGCAT_EXECUTABLE msgcat)
if (NOT GETTEXT_MSGCAT_EXECUTABLE )
  message (FATAL_ERROR "msgcat not found")
endif (NOT GETTEXT_MSGCAT_EXECUTABLE )

find_program (GETTEXT_MSGATTRIB_EXECUTABLE msgattrib)
if (NOT GETTEXT_MSGATTRIB_EXECUTABLE)
      message (FATAL_ERROR "msgattrib not found")
endif (NOT GETTEXT_MSGATTRIB_EXECUTABLE)

############ remove-potcdate.sed ###########
add_custom_command (
  OUTPUT remove-potcdate.sed
  COMMAND ${CMAKE_COMMAND} -DIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/remove-potcdate.sin -DOUT_FILE=remove-potcdate.sed -P ${CMAKE_CURRENT_SOURCE_DIR}/remove-commentlines.cmake
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/remove-potcdate.sin ${CMAKE_CURRENT_SOURCE_DIR}/remove-commentlines.cmake
  VERBATIM
)
############ POTFILES.in ###########

add_custom_command (
  OUTPUT POTFILES.in
  COMMAND po/update-po.sh ${CMAKE_CURRENT_BINARY_DIR}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Creating POTFILES.in"
)
set_source_files_properties (POTFILES.in PROPERTIES GENERATED TRUE)

############ warzone2100.pot ###########
# FIXME Change "Warzone Resurrection Project"
add_custom_command (
  OUTPUT warzone2100.pot
  COMMAND ${GETTEXT_XGETTEXT_EXECUTABLE}
    --default-domain=warzone2100
    --add-comments=TRANSLATORS:
    --from-code=UTF-8
    --no-wrap
    --width=1
    --keyword=_
    --keyword=N_
    --keyword=P_:1c,2
    --keyword=NP_:1c,2
    --files-from=${CMAKE_CURRENT_BINARY_DIR}/POTFILES.in
    --directory=${CMAKE_SOURCE_DIR}
    --output-dir=${CMAKE_BINARY_DIR}
    --copyright-holder="Warzone Resurrection Project"
    --package-name=warzone2100
    --msgid-bugs-address=warzone-dev@gna.org
  COMMAND ${CMAKE_COMMAND} -DREMOVE_POTCDATE_SED=po/remove-potcdate.sed -P ${CMAKE_CURRENT_SOURCE_DIR}/pot-update.cmake
  DEPENDS POTFILES.in remove-potcdate.sed
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Creating warzone2100.pot"
  VERBATIM
)
set_source_files_properties (warzone2100.pot PROPERTIES GENERATED TRUE)

############ *.po *.mo ###########

foreach (lang ${LANGUAGES})
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${lang}.po)
    message (WARNING "File ${lang}.po does not exist. If you are a translator, you can create it through 'msginit'.")
  else ()
    add_custom_command (
      OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${lang}.po-update
      COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} ${MSGMERGE_OPTIONS} -o ${CMAKE_CURRENT_SOURCE_DIR}/${lang}.new.po ${CMAKE_CURRENT_SOURCE_DIR}/${lang}.po warzone2100.pot
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -Dlang=${lang} -P po-update.cmake
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${lang}.po ${lang}.po
      DEPENDS warzone2100.pot
      COMMENT "Updating ${lang}.po"
    )
    if (NOT ${CMAKE_CURRENT_BINARY_DIR} EQUAL ${CMAKE_CURRENT_SOURCE_DIR})
      add_custom_command (
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${lang}.po
        COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} ${MSGMERGE_OPTIONS} -o ${lang}.new.po ${CMAKE_CURRENT_SOURCE_DIR}/${lang}.po warzone2100.pot
        COMMAND ${CMAKE_COMMAND} -Dlang=${lang} -P ${CMAKE_CURRENT_SOURCE_DIR}/po-update.cmake
        DEPENDS warzone2100.pot
        COMMENT "Generating updated ${lang}.po"
      )
      set_source_files_properties (${CMAKE_CURRENT_BINARY_DIR}/${lang}.po PROPERTIES GENERATED TRUE)
    endif ()

    add_custom_command (
       OUTPUT ${lang}.${MSGFMT_EXTENSION}
       COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${lang}.${MSGFMT_EXTENSION} ${CMAKE_CURRENT_BINARY_DIR}/${lang}.po
       DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${lang}.po
    )

    list (APPEND mo_FILES ${lang}.${MSGFMT_EXTENSION})
    list (APPEND po_FILES ${CMAKE_CURRENT_BINARY_DIR}/${lang}.po)
    list (APPEND po-update_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${lang}.po-update)

  endif ()
endforeach (lang)

# Make sure we don't remove the original po files with make clean.
set_directory_properties (PROPERTY CLEAN_NO_CUSTOM TRUE)
if (NOT ${CMAKE_CURRENT_BINARY_DIR} EQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  set (po-clean_FILES ${po_FILES})
endif ()
set_property (DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
  ${mo_FILES}
  ${po-clean_FILES}
  warzone2100.pot
  POTFILES.in
  remove-potcdate.sed
  stamp-po
)

add_custom_target (_gmo_target # internal target
  DEPENDS ${mo_FILES}
)
build_command (MAKE_MO_CMD
  TARGET _gmo_target
)
add_custom_command (
  OUTPUT stamp-po
  COMMAND ${CMAKE_COMMAND} -DMAKE_MO_CMD=${MAKE_MO_CMD} -P ${CMAKE_CURRENT_SOURCE_DIR}/stamp-po.cmake
  DEPENDS warzone2100.pot
)

############ Build targets ###########

if (BUILD_TRANSLATIONS)
  set (_all ALL)
else ()
  unset (_all)
endif ()
add_custom_target (translations
  ${_all}
  DEPENDS stamp-po
)

add_custom_target (po-update
  DEPENDS ${po-update_FILES}
)

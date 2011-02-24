set (script_HEADERS
  chat_processing.h
  codeprint.h
  event.h
  eventsave.h
  interpreter.h
  parse.h
  script.h
  stack.h
  CACHE INTERNAL ""
)

set (script_SOURCES
  codeprint.cpp
  event.cpp
  eventsave.cpp
  interpreter.cpp
  script.cpp
  stack.cpp
  CACHE INTERNAL ""
)

set (script_YACC
  chat
  script
  CACHE INTERNAL ""
)

set (script_FLEX
  chat
  script
  CACHE INTERNAL ""
)

if (WZ_LIBIFY)
	YACC_FLEX_TARGETS ("${script_YACC}" "${script_FLEX}" "" script_SOURCES)
	add_library (script STATIC ${script_SOURCES})
	#add_dependencies (script framework)
	target_link_libraries (script framework)
endif ()

# vim: ts=2:sw=2

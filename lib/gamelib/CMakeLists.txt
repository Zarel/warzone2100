set (gamelib_HEADERS
  anim.h
  animobj.h
  gtime.h
  hashtable.h
  parser.h
  CACHE INTERNAL ""
)

set (gamelib_SOURCES
  anim.cpp
  animobj.cpp
  gtime.cpp
  hashtable.cpp
  CACHE INTERNAL ""
)

set (gamelib_FLEX
  audp
  CACHE INTERNAL ""
)

set (gamelib_YACC
  audp
  CACHE INTERNAL ""
)

if (WZ_LIBIFY)
	YACC_FLEX_TARGETS ("${gamelib_YACC}" "${gamelib_FLEX}" "" gamelib_SOURCES)
	add_library (gamelib STATIC ${gamelib_SOURCES})
	#add_dependencies (gamelib ivis_opengl framework)
	target_link_libraries (gamelib ivis_opengl framework)
endif ()

# vim: ts=2:sw=2

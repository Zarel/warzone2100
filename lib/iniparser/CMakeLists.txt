set (iniparser_HEADERS
  iniparser.h
  CACHE INTERNAL ""
)

set (iniparser_SOURCES
  iniparser.cpp
  CACHE INTERNAL ""
)

if (WZ_LIBIFY)
	add_library (iniparser STATIC ${iniparser_SOURCES})
endif ()

#EXTRA_DIST = LICENSE AUTHORS



# vim: ts=2:sw=2

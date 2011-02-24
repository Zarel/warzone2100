set (netplay_HEADERS
  netlog.h
  netplay.h
  netqueue.h
  netsocket.h
  nettypes.h
  CACHE INTERNAL ""
)

set (netplay_SOURCES
  netjoin_stub.cpp
  netlog.cpp
  netplay.cpp
  netqueue.cpp
  netsocket.cpp
  nettypes.cpp
  CACHE INTERNAL ""
)

include_directories(${MINIUPNPC_INCLUDE_DIR})

if (WZ_LIBIFY)
	add_library (netplay STATIC ${netplay_SOURCES} ${netplay_YACC_FLEX_SOURCES})
	#add_dependencies (netplay framework miniupnpc)
	target_link_libraries (netplay framework miniupnpc)
endif ()

# vim: ts=2:sw=2

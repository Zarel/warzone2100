configure_file (${CMAKE_CURRENT_SOURCE_DIR}/miniupnpcstrings.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/miniupnpcstrings.h)
include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (miniupnpc_HEADERS
  miniupnpc.h
  ${CMAKE_CURRENT_BINARY_DIR}/miniupnpcstrings.h
  miniwget.h
  minixml.h
  minisoap.h
  minissdpc.h
  codelength.h
  upnpcommands.h
  igd_desc_parse.h
  upnpreplyparse.h
  upnperrors.h
  declspec.h
  bsdqueue.h
)

set (miniupnpc_SOURCES
  miniwget.c
  minixml.c
  igd_desc_parse.c
  minisoap.c
  miniupnpc.c
  upnpreplyparse.c
  upnpcommands.c
  minissdpc.c
  upnperrors.c
)

include_directories (${CMAKE_SOURCE_DIR})

add_library (miniupnpc STATIC ${miniupnpc_SOURCES})

#target_link_libraries (miniupnpc # For some reason we need the headers when we're on linux, but don't need to link...
#  ${PYTHON_LIBRARY}
#)

if (WIN32)
  target_link_libraries(miniupnpc ${WINSOCK2_LIBRARY})
endif ()

set(MINIUPNPC_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty CACHE PATH "")

# FIXME I tried all relevant properties from the miniupnpc target to get the name
#  but none worked. HACK in place until this is fixed

#get_target_property (_MINIUPNPC_NAME miniupnpc ARCHIVE_OUTPUT_NAME)
set (_MINIUPNPC_NAME "libminiupnpc.a")
get_target_property (_MINIUPNPC_DIR miniupnpc ARCHIVE_OUTPUT_DIRECTORY)
#message ("${_MINIUPNPC_NAME}")
set (MINIUPNPC_LIBRARY "${_MINIUPNPC_DIR}/${_MINIUPNPC_NAME}" CACHE PATH "")

#AM_CFLAGS = $(WZ_CFLAGS) -Wno-missing-declarations

set (ivis_opengl_HEADERS
  bitimage.h
  imd.h
  ivisdef.h
  pieblitfunc.h
  piedef.h
  piematrix.h
  piepalette.h
  pietypes.h
  rendmode.h
  tex.h
  ivi.h
  jpeg_encoder.h
  pieclip.h
  piefunc.h
  piemode.h
  piestate.h
  png_util.h
  screen.h
  textdraw.h
  CACHE INTERNAL ""
)

set (ivis_opengl_SOURCES
  bitimage.cpp
  imdload.cpp
  jpeg_encoder.cpp
  pieclip.cpp
  piefunc.cpp
  piemode.cpp
  piestate.cpp
  rendmode.cpp
  tex.cpp
  imd.cpp
  ivi.cpp
  pieblitfunc.cpp
  piedraw.cpp
  piematrix.cpp
  piepalette.cpp
  png_util.cpp
  screen.cpp
  textdraw.cpp
  CACHE INTERNAL ""
)

if (NOT SYSTEM_GLEE)
  set (ivis_opengl_HEADERS GLee.h ${ivis_opengl_HEADERS} CACHE INTERNAL "" FORCE)
  set (ivis_opengl_SOURCES GLee.c ${ivis_opengl_SOURCES} CACHE INTERNAL "" FORCE)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
endif (NOT SYSTEM_GLEE)

if (WZ_LIBIFY)
	add_library (ivis_opengl STATIC ${ivis_opengl_SOURCES})
	#add_dependencies (ivis_opengl framework)
	target_link_libraries (ivis_opengl framework)
endif ()

# vim: ts=2:sw=2

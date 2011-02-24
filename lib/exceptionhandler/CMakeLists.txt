set (exceptionhandler_HEADERS
  dumpinfo.h
  exceptionhandler.h
  include/coff/internal.h
  include/demangle.h
  include/libcoff.h
  CACHE INTERNAL ""
)

set (exceptionhandler_SOURCES
  dumpinfo.cpp
  exceptionhandler.cpp
  CACHE INTERNAL ""
)

if (WIN32 OR MINGW)
  set (exceptionhandler_HEADERS ${exceptionhandler_HEADERS} exchndl.h CACHE INTERNAL "" FORCE)
  set (exceptionhandler_SOURCES ${exceptionhandler_SOURCES} exchndl.cpp  CACHE INTERNAL "" FORCE)
endif ()

if (WZ_LIBIFY)
	add_library(exceptionhandler STATIC ${exceptionhandler_SOURCES})
endif ()

# vim: ts=2:sw=2

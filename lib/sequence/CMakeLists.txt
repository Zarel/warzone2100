set (sequence_HEADERS
  sequence.h
  timer.h
  CACHE INTERNAL ""
)


set (sequence_SOURCES
  sequence.cpp
  timer.cpp
  CACHE INTERNAL ""
)

if (WZ_LIBIFY)
	add_library (sequence STATIC ${sequence_SOURCES})
endif ()

# vim: ts=2:sw=2

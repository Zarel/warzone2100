set (sound_HEADERS
  aud.h
  audio.h
  audio_id.h
  cdaudio.h
  mixer.h
  playlist.h
  oggvorbis.h
  openal_error.h
  track.h
  tracklib.h
  CACHE INTERNAL ""
)

set (sound_SOURCES
  audio.cpp
  audio_id.cpp
  cdaudio.cpp
  oggvorbis.cpp
  openal_error.cpp
  openal_track.cpp
  playlist.cpp
  track.cpp
  CACHE INTERNAL ""
)

if (WZ_LIBIFY)
	add_library (sound STATIC ${sound_SOURCES})
	#add_dependencies (sound gamelib framework)
	target_link_libraries (sound gamelib framework)
endif ()

# vim: ts=2:sw=2

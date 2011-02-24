set (widget_HEADERS
  bar.h
  button.h
  editbox.h
  form.h
  label.h
  scrap.h
  slider.h
  tip.h
  widgbase.h
  widget.h
  widgint.h
  CACHE INTERNAL ""
)

set (widget_SOURCES
  bar.cpp
  button.cpp
  editbox.cpp
  form.cpp
  label.cpp
  scrap.cpp
  slider.cpp
  tip.cpp
  widget.cpp
  CACHE INTERNAL ""
)

if (WZ_LIBIFY)
	add_library (widget STATIC ${widget_SOURCES})
	#add_dependencies (widget ivis_opengl framework)
	target_link_libraries (widget ivis_opengl framework)
endif ()

# vim: ts=2:sw=2

set (framework_HEADERS
  configfile.h
  crc.h
  cursors.h
  debug.h
  file.h
  fixedpoint.h
  frame.h
  frameint.h
  frameresource.h
  i18n.h
  input.h
  lexer_input.h
  listmacs.h
  math_ext.h
  opengl.h
  physfs_ext.h
  resly.h
  stdio_ext.h
  string_ext.h
  strres.h
  strresly.h
  tagfile.h
  treap.h
  trig.h
  types.h
  utf.h
  vector.h
  config-macosx.h
  endian_hack.h
  gettext.h
  macros.h
  SDL_framerate.h
  wzapp_c.h
  wzglobal.h
  CACHE INTERNAL ""
)

set (framework_SOURCES
  configfile.cpp
  crc.cpp
  cursors16.cpp
  cursors32.cpp
  cursors.cpp
  debug.cpp
  frame.cpp
  frameresource.cpp
  i18n.cpp
  input.cpp
  lexer_input.cpp
  SDL_framerate.cpp
  stdio_ext.cpp
  strres.cpp
  tagfile.cpp
  treap.cpp
  trig.cpp
  utf.cpp
  CACHE INTERNAL ""
)

set (framework_YACC
  resource
  strres
  CACHE INTERNAL ""
)

set (framework_FLEX
  resource
  strres
  CACHE INTERNAL ""
)


if (WZ_LIBIFY)
	YACC_FLEX_TARGETS ("${framework_YACC}" "${framework_FLEX}" "" framework_SOURCES)
	add_library (framework STATIC ${framework_SOURCES})
endif ()

# vim: ts=2:sw=2

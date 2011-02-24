#file(GLOB warzone2100_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.hxx *.h )
#file(GLOB warzone2100_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cxx *.c )
set (warzone2100_HEADERS
  action.h
  actiondef.h
  advvis.h
  ai.h
  anim_id.h
  astar.h
  atmos.h
  basedef.h
  baseobject.h
  bridge.h
  bucket3d.h
  cheat.h
  challenge.h
  clparse.h
  cluster.h
  cmddroiddef.h
  cmddroid.h
  combat.h
  component.h
  configuration.h
  console.h
  data.h
  design.h
  difficulty.h
  display3ddef.h
  display3d.h
  displaydef.h
  display.h
  drive.h
  droiddef.h
  droid.h
  e3demo.h
  edit3d.h
  effects.h
  featuredef.h
  feature.h
  fpath.h
  frend.h
  frontend.h
  functiondef.h
  function.h
  game.h
  gateway.h
  geometry.h
  group.h
  hci.h
  ingameop.h
  init.h
  intdisplay.h
  intelmap.h
  intfac.h
  intimage.h
  intorder.h
  keybind.h
  keyedit.h
  keymap.h
  levelint.h
  levels.h
  lighting.h
  loadsave.h
  loop.h
  main.h
  mapdisplay.h
  mapgrid.h
  map.h
  mechanics.h
  messagedef.h
  message.h
  messagely.h
  miscimd.h
  missiondef.h
  mission.h
  modding.h
  movedef.h
  move.h
  multigifts.h
  multiint.h
  multijoin.h
  multilimit.h
  multimenu.h
  multiplay.h
  multirecv.h
  multistat.h
  objectdef.h
  objects.h
  objmem.h
  oprint.h
  orderdef.h
  order.h
  parsetest.h
  pointtree.h
  positiondef.h
  power.h
  projectiledef.h
  projectile.h
  radar.h
  random.h
  raycast.h
  researchdef.h
  research.h
  scores.h
  scriptai.h
  scriptcb.h
  scriptextern.h
  scriptfuncs.h
  scriptobj.h
  scripttabs.h
  scriptvals.h
  selection.h
  seqdisp.h
  statsdef.h
  stats.h
  stringdef.h
  structuredef.h
  structure.h
  terrain.h
  text.h
  texture.h
  transporter.h
  visibility.h
  version.h
  warcam.h
  warzoneconfig.h
  wavecast.h
  weapondef.h
  wrappers.h
)

set (warzone2100_SOURCES
  action.cpp
  advvis.cpp
  ai.cpp
  astar.cpp
  atmos.cpp
  aud.cpp
  baseobject.cpp
  bridge.cpp
  bucket3d.cpp
  challenge.cpp
  cheat.cpp
  clparse.cpp
  cluster.cpp
  cmddroid.cpp
  combat.cpp
  component.cpp
  configuration.cpp
  console.cpp
  data.cpp
  design.cpp
  difficulty.cpp
  display3d.cpp
  display.cpp
  drive.cpp
  droid.cpp
  e3demo.cpp
  edit3d.cpp
  effects.cpp
  feature.cpp
  fpath.cpp
  frontend.cpp
  function.cpp
  game.cpp
  gateway.cpp
  geometry.cpp
  group.cpp
  hci.cpp
  ingameop.cpp
  init.cpp
  intdisplay.cpp
  intelmap.cpp
  intimage.cpp
  intorder.cpp
  keybind.cpp
  keyedit.cpp
  keymap.cpp
  levels.cpp
  lighting.cpp
  loadsave.cpp
  loop.cpp
  main.cpp
  map.cpp
  mapdisplay.cpp
  mapgrid.cpp
  mechanics.cpp
  message.cpp
  miscimd.cpp
  mission.cpp
  move.cpp
  multibot.cpp
  multigifts.cpp
  multiint.cpp
  multijoin.cpp
  multilimit.cpp
  multimenu.cpp
  multiopt.cpp
  multiplay.cpp
  multistat.cpp
  multistruct.cpp
  multisync.cpp
  objects.cpp
  objmem.cpp
  oprint.cpp
  order.cpp
  parsetest.cpp
  pointtree.cpp
  power.cpp
  projectile.cpp
  radar.cpp
  random.cpp
  raycast.cpp
  research.cpp
  scores.cpp
  scriptai.cpp
  scriptcb.cpp
  scriptextern.cpp
  scriptfuncs.cpp
  scriptobj.cpp
  scripttabs.cpp
  scriptvals.cpp
  selection.cpp
  seqdisp.cpp
  stats.cpp
  structure.cpp
  terrain.cpp
  text.cpp
  texture.cpp
  transporter.cpp
  version.cpp
  visibility.cpp
  warcam.cpp
  warzoneconfig.cpp
  wavecast.cpp
  wrappers.cpp
)

set (warzone2100_YACC
  message
  scriptvals
)

set (warzone2100_FLEX
  message
  scriptvals
  level
)

YACC_FLEX_TARGETS("${warzone2100_YACC}" "${warzone2100_FLEX}" "" warzone2100_SOURCES)

if (NOT WZ_LIBIFY)
	set (warzone2100_LIBDIRS
		framework
		exceptionhandler
		gamelib
		iniparser
		ivis_opengl
		netplay
		script
		sequence
		sound
		widget
	)

	foreach (subdir ${warzone2100_LIBDIRS})
		foreach (source ${${subdir}_SOURCES})
		  set (warzone2100_SOURCES ${warzone2100_SOURCES} ${CMAKE_SOURCE_DIR}/lib/${subdir}/${source})
		endforeach ()
		if (${subdir}_YACC) # Currently if ${subdir}_YACC is set, then so is ${subdir}_FLEX, so save on complexity
			YACC_FLEX_TARGETS("${${subdir}_YACC}" "${${subdir}_FLEX}" "${CMAKE_SOURCE_DIR}/lib/${subdir}/" warzone2100_SOURCES)
		endif ()
	endforeach ()
endif (NOT WZ_LIBIFY)

add_executable (warzone2100 ${warzone2100_SOURCES})

if (WZ_LIBIFY)
	target_link_libraries (warzone2100
		widget
		sequence
		sound
		script
		netplay
		ivis_opengl
		gamelib
		framework
		iniparser
		exceptionhandler
	)
else ()
	target_link_libraries (warzone2100 miniupnpc)
  include_directories (${MINIUPNPC_INCLUDE_DIR})
endif ()

target_link_libraries (warzone2100
  ${ZLIB_LIBRARY}
  ${SDL_LIBRARY}
  ${PNG_LIBRARY}
  ${SDLNET_LIBRARY}
  ${OPENAL_LIBRARY}
  ${PHYSFS_LIBRARY}
  ${GETTEXT_LIBRARIES}
  ${GLC_LIBRARIES}
  ${OPENGL_LIBRARY}
  ${THEORA_LIBRARY}
  ${OGG_LIBRARY}
  ${VORBIS_LIBRARY}
  ${VORBISFILE_LIBRARY}
)

if (NEED_DEP_DEPS)
  target_link_libraries (warzone2100
    ${FONTCONFIG_LIBRARIES}
    ${FREETYPE2_LIBRARIES}
    ${ICONV_LIBRARIES}
    ${EXPAT_LIBRARY}
  )
endif (NEED_DEP_DEPS)

if (WIN32)
  target_link_libraries(warzone2100
    ${WINMM_LIBRARY}
    ${WINSOCK2_LIBRARY}
    ${BFD_LIBRARY}
    ${DBGHELP_LIBRARY}
    ${IBERTY_LIBRARY}
    ${SHFOLDER_LIBRARY}
    ${SHLWAPI_LIBRARY}
    ${PSAPI_LIBRARY}
)
else ()
  target_link_libraries(warzone2100
    ${JPEG_LIBRARY}
	)
endif ()

# vim: ts=2:sw=2

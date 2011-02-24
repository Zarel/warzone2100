set (lib_SUBDIRS
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
foreach (subdir ${lib_SUBDIRS})
	add_subdirectory (${subdir})
endforeach ()

# vim: ts=2:sw=2

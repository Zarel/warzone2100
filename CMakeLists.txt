##################################
# Warzone 2100 CMake build system
##################################

##################################
# Basic CMake Configuration
##################################
cmake_minimum_required (VERSION 2.6)
set (CMAKE_COLOR_MAKEFILE TRUE)
set (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy (SET CMP0003 NEW)
cmake_policy (SET CMP0005 NEW)

# Use relative paths
# This is mostly to reduce path size for command-line limits on windows
if (WIN32)
  # This seems to break Xcode projects so definitely don't enable on Apple builds
  set (CMAKE_USE_RELATIVE_PATHS true)
  set (CMAKE_SUPPRESS_REGENERATION true)
endif ()

project (warzone2100) # CXX and C are default languages.

# Include Submodules
set (CMAKE_MODULE_PATH 
  ${CMAKE_SOURCE_DIR}/cmake/modules
)
#include (AddFileDependencies)
include (CheckCXXCompilerFlag)
include (CheckFunctionExists)
include (CheckIncludeFiles)
#include (CheckLibraryExists)
#include (CMakeDependentOption)
include (FindPkgConfig)
#include (PreprocessorUtils)
#include (TestBigEndian)

#####################################################################
# Set up the basic build environment
#####################################################################

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# Auto version TODO put autorevision in its own module.
find_package (Git)
if (GIT_FOUND)
  execute_process (
    COMMAND ${GIT_EXECUTABLE} diff --quiet HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE WC_MODIFIED
    OUTPUT_QUIET
    ERROR_QUIET
  )
  execute_process (
    COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE SVN_REV
    RESULT_VARIABLE _REV_LIST_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if (NOT _REV_LIST_RESULT EQUAL 0)
    set (SVN_REV -42)
  endif ()
  execute_process (
    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE SVN_REV_STR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  string(SUBSTRING "${SVN_REV_STR}" 0 7 SVN_SHORT_HASH)
  execute_process (
    COMMAND ${GIT_EXECUTABLE} symbolic-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE SVN_URI
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  execute_process (
    COMMAND ${GIT_EXECUTABLE} describe --exact-match --tags
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE SVN_TAG
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if (SVN_TAG STREQUAL "")
    set (SVN_TAG ${SVN_URI})
  endif ()
  execute_process (
    COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%ci
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE SVN_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else ()
  set (SVN_REV -42)
  set (SVN_REV_STR -42)
  set (SVN_DATE "1900-01-01 00:00:00 0000")
  set (SVN_URI  -42)
  set (SVN_TAG -42)
endif ()
configure_file (${CMAKE_SOURCE_DIR}/autorevision.h.cmake ${CMAKE_BINARY_DIR}/autorevision.h)
configure_file (${CMAKE_SOURCE_DIR}/autorevision.h.cmake ${CMAKE_BINARY_DIR}/src/autorevision.h) # autotools compatibility...

if (MSVC)
  if (CMAKE_BUILD_TOOL STREQUAL "nmake")
    # set variable to state that we are using nmake makefiles
  set (NMAKE TRUE)
  endif ()
endif ()

if (MINGW)
  add_definitions (-D_WIN32_WINNT=0x0500)
endif ()

add_definitions (-D_REENTRANT -DYY_STATIC -DYY_NO_INPUT -DSTATICLIB -DVERSION="TRUNK" -DPACKAGE="warzone2100")

if (UNIX AND NOT WIN32)
  string (REGEX MATCH [fF][rR][eE]{2}[bB][sS][dD] IS_FREEBSD ${CMAKE_SYSTEM_NAME})
  if (NOT IS_FREEBSD)
    add_definitions (-D_XOPEN_SOURCE=600)
  endif ()
endif ()

# Set compiler specific build flags
if (CMAKE_COMPILER_IS_GNUCXX)
  # Set our own default flags at first run.
  if (NOT CONFIGURED)

    if (ENABLE_STRICT_COMPILATION)
      # The current networking code breaks strict aliasing in g++ 4.5.
      set(STRICT_FLAGS "-Werror -Wno-strict-aliasing")
    else (ENABLE_STRICT_COMPILATION)
      set(STRICT_FLAGS "")
    endif (ENABLE_STRICT_COMPILATION)

    # TODO - Special stack flag checks

    set(CMAKE_C_FLAGS "-Wall $ENV{CFLAGS}" # CMAKE_C_FLAGS gets appended to the other C flags
        CACHE STRING "Flags used by the C compiler during normal builds." FORCE)
    set(CMAKE_C_FLAGS_DEBUG "-g -DDDEBUG"
        CACHE STRING "Flags used by the C compiler during debug builds." FORCE)
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG"
        CACHE STRING "Flags used by the C compiler during release builds." FORCE)
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG"
        CACHE STRING "Flags used by the C compiler during release builds." FORCE)
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG"
        CACHE STRING "Flags used by the C compiler during release builds." FORCE)

    set(CMAKE_CXX_FLAGS "-Wall -fno-rtti -fno-exceptions ${STRICT_FLAGS} $ENV{CXXFLAGS}" # CMAKE_CXX_FLAGS gets appended to the other CXX flags
        CACHE STRING "Flags used by the CXX compiler during normal builds." FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG"
        CACHE STRING "Flags used by the CXX compiler during debug builds." FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG"
        CACHE STRING "Flags used by the CXX compiler during release builds." FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG"
        CACHE STRING "Flags used by the CXX compiler during release builds." FORCE)
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG"
        CACHE STRING "Flags used by the CXX compiler during release builds." FORCE)

  endif (NOT CONFIGURED)
endif ()

#######################################################################
# Dependencies
#######################################################################

find_package (PNG)
find_package (SDL)
find_package (OpenGL)
find_package (OpenAL)
find_package (PhysFS)
find_package (Gettext)
find_package (Theora)
find_package (FLEX)
find_package (YACC)
find_package (OggVorbis)
find_package (ZLIB)

# use pkg-config because adds the secondary requirements of quesoglc
pkg_check_modules(GLC REQUIRED quesoglc)


# TODO Use system GLee

# Add library and include paths from the dependencies
include_directories(
  ${PNG_INCLUDE_DIR}
  ${SDL_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${OPENAL_INCLUDE_DIR}
  ${PHYSFS_INCLUDE_DIR}
  ${GETTEXT_INCLUDE_DIR}
  ${THEORA_INCLUDE_DIR}
  ${GLC_INCLUDE_DIRS}
  ${OGGVORBIS_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/src/ivis_opengl # GLee.h
)

if (WIN32)
  find_library (WINSOCK2_LIBRARY
    NAMES
      ws2_32 WS2_32
  )
  find_library (WINMM_LIBRARY
    NAMES
      Winmm winmm
  )
  find_library (DBGHELP_LIBRARY
    NAMES
      DbgHelp dbghelp
  )
  find_library (BFD_LIBRARY
    NAMES
      bfd
  )
  find_library (IBERTY_LIBRARY
    NAMES
      iberty
  )
  find_library (SHFOLDER_LIBRARY
    NAMES
      shfolder
  )
  find_library (SHLWAPI_LIBRARY
    NAMES
      shlwapi
  )
  find_library (PSAPI_LIBRARY
    NAMES
      psapi
  )
else (WIN32)
  find_package (JPEG) # FIXME add libjpeg to the windows CC dev pkg and move this to a general dep
  find_package (PythonLibs) # for miniupnpc

  include_directories (
    ${JPEG_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR} # miniupnpc
  )
endif (WIN32)


# TODO finish this
CHECK_INCLUDE_FILES (alloca.h HAVE_ALLOCA_H)
CHECK_FUNCTION_EXISTS (CFLocaleCopyCurrent HAVE_CFLOCALECOPYCURRENT)
CHECK_FUNCTION_EXISTS (CFPreferencesCopyAppValue HAVE_CFPREFERENCESCOPYAPPVALUE)
CHECK_FUNCTION_EXISTS (dcgettext HAVE_DCGETTEXT)
CHECK_FUNCTION_EXISTS (gettext HAVE_GETTEXT)
CHECK_FUNCTION_EXISTS (iconv HAVE_ICONV)
CHECK_INCLUDE_FILES (inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES (memory.h HAVE_MEMORY_H)
CHECK_INCLUDE_FILES (stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES (stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES (strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILES (string.h HAVE_STRING_H)
CHECK_FUNCTION_EXISTS (strlcpy HAVE_SYSTEM_STRLCPY)
CHECK_FUNCTION_EXISTS (strlcat HAVE_SYSTEM_STRLCAT)
CHECK_INCLUDE_FILES (sys/stat.h HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES (sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES (unistd.h HAVE_UNISTD_H)

configure_file (${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)
include_directories (${CMAKE_BINARY_DIR})

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Note that static libraries are treated as archive targets
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

######################################################################
# Provide user options to customise the build process
######################################################################
option (ENABLE_NLS "Enable translation targets, requires command line utilities: find, sh, sed." TRUE)
option (BUILD_TRANSLATIONS "Make translations with default target." FALSE)
option (DISABLE_SOUND "Disables sound" FALSE)
option (WZ_LIBIFY "Build everything in the lib directory as libraries and use them to link warzone." TRUE)

# hide advanced options
mark_as_advanced(WZ_LIBIFY)
###################################################################
# configure global build settings based on selected build options
###################################################################
if (DISABLE_SOUND)
  add_definitions (-DWZ_NOSOUND)
endif (DISABLE_SOUND)

MACRO(YACC_FLEX_TARGETS Y_FILES L_FILES BASEPATH OUTPUT) # FIXME move this to its own file
	foreach (y_file ${Y_FILES})
		YACC_TARGET (${y_file}_parser "${BASEPATH}${y_file}_parser.y" ${CMAKE_CURRENT_BINARY_DIR}/${y_file}_parser.tab.cpp)
		set (${OUTPUT} ${${OUTPUT}} "${YACC_${y_file}_parser_OUTPUTS}" CACHE INTERNAL "")
		set_source_files_properties ("${YACC_${y_file}_parser_OUTPUTS}" PROPERTIES GENERATED true)
	endforeach()

	foreach (l_file ${L_FILES})
		FLEX_TARGET (${l_file}_lexer "${BASEPATH}${l_file}_lexer.l" ${CMAKE_CURRENT_BINARY_DIR}/${l_file}_lexer.lex.cpp)
		set (${OUTPUT} ${${OUTPUT}} "${FLEX_${l_file}_lexer_OUTPUTS}" CACHE INTERNAL "")
		set_source_files_properties ("${FLEX_${l_file}_lexer_OUTPUTS}" PROPERTIES GENERATED true)
		#list (FIND Y_FILES l_file index)
		foreach (y_file ${Y_FILES})
		  if (y_file STREQUAL l_file)
		    set_source_files_properties("${FLEX_${l_file}_lexer_OUTPUTS}"
		      PROPERTIES OBJECT_DEPENDS "${YACC_${l_file}_parser_OUTPUT_HEADER}")
		    break ()
		  endif ()
		endforeach ()
	endforeach ()
ENDMACRO ()

##################################################################
# Now setup targets
##################################################################
add_subdirectory (3rdparty)
include_directories (${CMAKE_SOURCE_DIR})
add_subdirectory (lib)
add_subdirectory (src)
if (ENABLE_NLS)
  add_subdirectory (po)
endif (ENABLE_NLS)

## Other stuff
set (CONFIGURED YES CACHE INTERNAL "")

# vim: ts=2:sw=2

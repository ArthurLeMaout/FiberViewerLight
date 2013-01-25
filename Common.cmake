include(CMakeDependentOption)
include( setSlicerConfigVarList )
enable_language(C)
enable_language(CXX)



set(Slicer_SKIP_PROJECT_COMMAND ON)
#-----------------------------------------------------------------------------
set(EXTENSION_NAME FiberViewerLight)
set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/FiberViewerLight")
set(EXTENSION_CATEGORY "Diffusion")
set(EXTENSION_CONTRIBUTORS "Francois Budin (UNC)")
set(EXTENSION_DESCRIPTION "This extension provides the tool FiberViewerLight integrated in Slicer")
set(EXTENSION_ICONURL "http://www.nitrc.org/project/list_screenshots.php?group_id=534&screenshot_id=598")
set(EXTENSION_SCREENSHOTURLS "http://www.nitrc.org/project/list_screenshots.php?group_id=534&screenshot_id=599 http://www.nitrc.org/project/list_screenshots.php?group_id=534&screenshot_id=597")
set(EXTENSION_STATUS "Beta")
set(EXTENSION_DEPENDS "NA") # Specified as a space separated list or 'NA' if any
set(EXTENSION_BUILD_SUBDIRECTORY fvlight-build)

find_package(Git REQUIRED)

option( BUILD_TESTING   "Build the testing tree" ON )


# With CMake 2.8.9 or later, the UPDATE_COMMAND is required for updates to occur.
# For earlier versions, we nullify the update state to prevent updates and
# undesirable rebuild.
if(CMAKE_VERSION VERSION_LESS 2.8.9)
  set(cmakeversion_external_update UPDATE_COMMAND "")
else()
  set(cmakeversion_external_update LOG_UPDATE 1)
endif()
 
#-----------------------------------------------------------------------------
# Update CMake module path
#------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake
  ${CMAKE_CURRENT_BINARY_DIR}/CMake
  ${CMAKE_MODULE_PATH}
  )

#-----------------------------------------------------------------------------
# Sanity checks
#------------------------------------------------------------------------------
include(PreventInSourceBuilds)
include(PreventInBuildInstalls)

#-----------------------------------------------------------------------------
# CMake Function(s) and Macro(s)
#-----------------------------------------------------------------------------
if(CMAKE_PATCH_VERSION LESS 3)
  include(Pre283CMakeParseArguments)
else()
  include(CMakeParseArguments)
endif()


macro( unsetForSlicer )
  set(options VERBOSE )
  set(oneValueArgs )
  set(multiValueArgs NAMES )
  CMAKE_PARSE_ARGUMENTS(TMP
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )
  foreach( var ${TMP_NAMES} )
    if( TMP_VERBOSE )
      message( "Unsetting "${var} - old value: ${${var}} )
    endif()
    set( ${var}_TMP ${${var}} )
    unset( ${var} CACHE )
    unset( ${var} )
  endforeach()
endmacro()

macro( resetForSlicer )
  set(options VERBOSE )
  set(oneValueArgs )
  set(multiValueArgs NAMES )
  CMAKE_PARSE_ARGUMENTS(TMP
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )
  foreach( var ${TMP_NAMES} )
    set( ${var} ${${var}_TMP} CACHE PATH "${var} PATH" FORCE )
    unset( ${var}_TMP )
    if( TMP_VERBOSE )
      message( "resetting ${var} - new value: "${${var}} )
    endif()
  endforeach()
endmacro()

macro( unsetAllForSlicerBut )
  set(options VERBOSE )
  set(oneValueArgs )
  set(multiValueArgs NAMES )
  CMAKE_PARSE_ARGUMENTS(TMP
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )
  foreach( var ${TMP_NAMES} )
    if( TMP_VERBOSE )
      message( "Saving ${var} - value: ${${var}}" )
    endif()
    set( ${var}_TMP ${${var}} )
  endforeach()
  foreach( var ${SlicerConfigVarList} )
    unset( ${var} CACHE )
    unset( ${var} )
  endforeach()
  foreach( var ${TMP_NAMES} )
    if( TMP_VERBOSE )
      message( "Writing back ${var} - value: ${${var}_TMP}")
    endif()
    set( ${var} ${${var}_TMP} )
  endforeach()
endmacro()

#-------------------------------------------------------------------------
# Augment compiler flags
#-------------------------------------------------------------------------
include(ITKSetStandardCompilerFlags)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_DEBUG_DESIRED_FLAGS}" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_DEBUG_DESIRED_FLAGS}" )
else() # Release, or anything else
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_RELEASE_DESIRED_FLAGS}" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_RELEASE_DESIRED_FLAGS}" )
endif()

#-----------------------------------------------------------------------------
# Add needed flag for gnu on linux like enviroments to build static common libs
# suitable for linking with shared object libs.
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  if(NOT "${CMAKE_CXX_FLAGS}" MATCHES "-fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  endif()
  if(NOT "${CMAKE_C_FLAGS}" MATCHES "-fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  endif()
endif()

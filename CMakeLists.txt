project(FiberViewerLight)
cmake_minimum_required(VERSION 2.8.7)

#-----------------------------------------------------------------------------
set(LOCAL_PROJECT_NAME FiberViewerLight)

include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)

#option( FiberViewerLight_BUILD_SLICER_EXTENSION
#        "Build FiberViewerLight as a Slicer extension or as a Superbuild project" OFF )
set(LOCAL_PROJECT_NAME FiberViewerLight)
option(${LOCAL_PROJECT_NAME}_SUPERBUILD "Build ${LOCAL_PROJECT_NAME} and the projects it depends on via SuperBuild.cmake." ON)
mark_as_advanced(${LOCAL_PROJECT_NAME}_SUPERBUILD)

if( ${LOCAL_PROJECT_NAME}_SUPERBUILD )
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
else()
  include("${CMAKE_CURRENT_SOURCE_DIR}/FiberViewerLight.cmake")
endif()



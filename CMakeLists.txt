project(FiberViewerLight)
cmake_minimum_required(VERSION 3.5)

set(PRIMARY_PROJECT_NAME FiberViewerLight)

include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)

#option( FiberViewerLight_BUILD_SLICER_EXTENSION
#        "Build FiberViewerLight as a Slicer extension or as a Superbuild project" OFF )
set(PRIMARY_PROJECT_NAME FiberViewerLight)
option(${PRIMARY_PROJECT_NAME}_SUPERBUILD "Build ${PRIMARY_PROJECT_NAME} and the projects it depends on via SuperBuild.cmake." ON)

if( ${PRIMARY_PROJECT_NAME}_SUPERBUILD )
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
else()
  include("${CMAKE_CURRENT_SOURCE_DIR}/FiberViewerLight.cmake")
endif()



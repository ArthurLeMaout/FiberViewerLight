cmake_minimum_required(VERSION 2.8.7)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  set(EXTENSION_NAME FiberViewerLight)
  set(EXTENSION_HOMEPAGE
"http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/FiberViewerLight")
  set(EXTENSION_CATEGORY "Diffusion")
  set(EXTENSION_CONTRIBUTORS "Francois Budin (UNC)")
  set(EXTENSION_DESCRIPTION "This extension provides the tool FiberViewerLight
integrated in Slicer")
  set(EXTENSION_ICONURL "http://www.nitrc.org/project/list_screenshots.php?group_id=534&screenshot_id=598")
  set(EXTENSION_SCREENSHOTURLS 
"http://www.nitrc.org/project/list_screenshots.php?group_id=534&screenshot_id=599
http://www.nitrc.org/project/list_screenshots.php?group_id=534&screenshot_id=597")
  set(EXTENSION_STATUS "Beta")
  set(EXTENSION_DEPENDS "NA") # Specified as a space separated list or 'NA' if any
  set(EXTENSION_BUILD_SUBDIRECTORY fvlight-build)
endif()

#-----------------------------------------------------------------------------
set(MODULE_NAME FiberViewerLight)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
 find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()


include(ExternalProject)
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

option(USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)
set(git_protocol "git")
if(NOT USE_GIT_PROTOCOL)
  set(git_protocol "http")
else(NOT USE_GIT_PROTOCOL)
  set(git_protocol "git")
endif()

set(proj VTK)
set(vtk_tag v5.10.0)

set(vtk_GUI_ARGS
    -DVTK_USE_GUISUPPORT:BOOL=ON
    -DVTK_USE_QVTK:BOOL=ON
    -DVTK_USE_QT:BOOL=ON
    -DVTK_USE_X:BOOL=OFF
    -DVTK_USE_CARBON:BOOL=OFF
    -DVTK_USE_COCOA:BOOL=ON
    -DVTK_USE_RENDERING:BOOL=ON
  )
if(APPLE)
  # Qt 4.6 binary libs are built with empty OBJCXX_FLAGS for mac Cocoa
  set(vtk_GUI_ARGS
      ${vtk_GUI_ARGS}
     -DVTK_REQUIRED_OBJCXX_FLAGS:STRING=
     )
endif(APPLE)
set(${proj}_REPOSITORY ${git_protocol}://vtk.org/VTK.git CACHE STRING "" FORCE)
ExternalProject_Add(${proj}
      GIT_TAG ${vtk_tag}
      GIT_REPOSITORY ${${proj}_REPOSITORY}
      UPDATE_COMMAND ""
      SOURCE_DIR ${proj}
      BINARY_DIR ${proj}-build
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        -DBUILD_SHARED_LIBS:BOOL=OFF
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DVTK_USE_PARALLEL:BOOL=OFF
        -DVTK_DEBUG_LEAKS:BOOL=OFF
        -DVTK_WRAP_TCL:BOOL=OFF
        -DVTK_WRAP_PYTHON:BOOL=OFF
        -DVTK_USE_QTCHARTS:BOOL=ON
        -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        ${vtk_GUI_ARGS}
      INSTALL_COMMAND ""
    )
set( VTK_DIR ${CMAKE_CURRENT_BINARY_DIR}/${proj}-build )

set(proj QWT)
ExternalProject_Add(${proj}
  SVN_REPOSITORY http://svn.code.sf.net/p/qwt/code/branches/qwt-6.0/
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}
  SVN_REVISION -r 1599
  DEPENDS ""
  CMAKE_GENERATOR ${gen}
  CONFIGURE_COMMAND ${QT_QMAKE_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${proj}
  INSTALL_COMMAND ""
)
#install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/QWT/lib/ DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_LIBRARY_DESTINATION} )

set(proj fvlight)
ExternalProject_Add(${proj}
  SVN_REPOSITORY https://www.nitrc.org/svn/fvlight/trunk
  SVN_USERNAME slicerbot
  SVN_PASSWORD slicer
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  SVN_REVISION -r 32
  DEPENDS QWT VTK
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${LOCAL_CMAKE_BUILD_OPTIONS}
    -DVTK_DIR:PATH=${VTK_DIR}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DQWT_LIB_PATH:FILEPATH=${CMAKE_CURRENT_BINARY_DIR}/QWT/lib
    -DQWT_INCLUDE_DIR_GIVEN:PATH=${CMAKE_CURRENT_BINARY_DIR}/QWT/src
    -DITK_DIR:PATH=${ITK_DIR}
    -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    -DMIDAS_PACKAGE_EMAIL:STRING=${MIDAS_PACKAGE_EMAIL}
    -DMIDAS_PACKAGE_API_KEY:STRING=${MIDAS_PACKAGE_API_KEY}
    -DEXTENSION_NAME:STRING=${EXTENSION_NAME}
    -DEXTENSION_SUPERBUILD_BINARY_DIR:PATH=${${EXTENSION_NAME}_BINARY_DIR}
    # Slicer
    -DSlicer_DIR:PATH=${Slicer_DIR}
    -DSlicer_USE_FILE:FILEPATH=${Slicer_USE_FILE}
    -DSlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY:PATH=${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY}
    -DSlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION:PATH=${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION}
    -DSlicerExecutionModel_DEFAULT_CLI_INSTALL_LIBRARY_DESTINATION:PATH=${SlicerExecutionModel_DEFAULT_CLI_INSTALL_LIBRARY_DESTINATION}
    INSTALL_COMMAND ""
)


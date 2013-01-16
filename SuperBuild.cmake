cmake_minimum_required(VERSION 2.8.7)

include(ExternalProject)
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

find_package(Git REQUIRED)

option(USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)
set(git_protocol "git")
if(NOT USE_GIT_PROTOCOL)
  set(git_protocol "http")
else(NOT USE_GIT_PROTOCOL)
  set(git_protocol "git")
endif()

#-----------------------------------------------------------------------------

unset( VTK_DIR CACHE )
unset( VTK_DIR )
unset( ITK_DIR CACHE )
unset( ITK_DIR )
unset( SlicerExecutionModel_DIR CACHE )
unset( SlicerExecutionModel_DIR )
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

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
  DOWNLOAD_COMMAND ""
  INSTALL_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR ${proj}-build
  DEPENDS QWT VTK
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
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
    -DADDITIONAL_C_FLAGS:STRING=${ADDITIONAL_C_FLAGS}
    -DADDITIONAL_CXX_FLAGS:STRING=${ADDITIONAL_CXX_FLAGS}
    -DEXTENSION_NAME:STRING=${EXTENSION_NAME}
    -DEXTENSION_SUPERBUILD_BINARY_DIR:PATH=${${EXTENSION_NAME}_BINARY_DIR}
    -DFiberViewerLight_BUILD_SLICER_EXTENSION:BOOL=OFF
    # Slicer
    -DSlicer_DIR:PATH=${Slicer_DIR}
    -DSlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY:PATH=${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY}
    -DSlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION:PATH=${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION}
    -DSlicerExecutionModel_DEFAULT_CLI_INSTALL_LIBRARY_DESTINATION:PATH=${SlicerExecutionModel_DEFAULT_CLI_INSTALL_LIBRARY_DESTINATION}
)


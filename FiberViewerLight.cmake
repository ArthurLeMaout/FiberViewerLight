#-----------------------------------------------------------------------------
set(MODULE_NAME ${EXTENSION_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)


FIND_PACKAGE(VTK REQUIRED)
IF (VTK_FOUND)
#  SET(VTK_USE_QVTK TRUE)
#  SET(VTK_USE_GUISUPPORT TRUE)
  INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
   MESSAGE(FATAL_ERROR, "VTK not found. Please set VTK_DIR.")
ENDIF (VTK_FOUND)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
	MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR")
ENDIF(ITK_FOUND)

FIND_PACKAGE(Qt4 REQUIRED)
IF(QT_USE_FILE)
  INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
   MESSAGE(FATAL_ERROR, "QT not found. Please set QT_DIR.")
ENDIF(QT_USE_FILE)

# Qwt include directory

FIND_PATH(QWT_INCLUDE_DIR qwt.h
  ${QWT_INCLUDE_DIR_GIVEN}
  /usr/local/qwt/include
  /usr/local/include
  /usr/include/qwt
  /usr/include
)


if(NOT QWT_INCLUDE_DIR)
  message(FATAL_ERROR "QWT_INCLUDE_DIR not found. It should be set to a path containing qwt.h, like '/Projects/qwt-6.0.1-svn/include'")
endif()

# Qwt libraries
SET(QWT_NAMES ${QWT_NAMES} qwt libqwt)
FIND_LIBRARY(QWT_LIBRARY
  NAMES ${QWT_NAMES}
  PATHS ${QWT_LIB_PATH} /usr/local/qwt/lib /usr/local/lib /usr/lib
)
if(NOT QWT_LIBRARY)
  message(FATAL_ERROR "QWT_LIBRARY not found. It should be set to a filepath like '/Projects/qwt-6.0.1-svn/lib/libqwt.a'")
endif()
set(QWT_LIBRARIES ${QWT_LIBRARY})

find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})


QT4_WRAP_CPP(MOC_FILES FiberViewerLightGUI.h FVLengthGUI.h FVDistributionGUI.h FVPanelGUI.h FVDisplayClassGUI.h FVCutterGUI.h FVNormalizedCutGUI.h PlanSetting.h FiberDisplay.h)

SET(FVLight_source FiberViewerLight.cxx FiberViewerLightGUI.cxx FiberDisplay.cxx FVLengthGUI.cxx  FVDistributionGUI.cxx FVPanelGUI.cxx FVDisplayClassGUI.cxx FVNormalizedCutGUI.cxx PlanSetting.cxx FVCutterGUI.cxx ${MOC_FILES})
SET(FVLight_header FiberViewerLightGUI.h FiberDisplay.h FVLengthGUI.h FVDistributionGUI.h FVPanelGUI.h FVDisplayClassGUI.h FVNormalizedCutGUI.h PlanSetting.h FVCutterGUI.h)

# GENERATECLP(FiberViewerLight.cxx FiberViewerLight.xml)
#add_executable( FiberViewerLight ${FVLight_source} ${FVLight_header})

set(VTK_LIBRARIES 
  vtkWidgets
  vtkRendering
  vtkGraphics
  vtkImaging
  vtkIO
  vtkFiltering
  vtkCommon
  vtkHybrid
  vtksys
  QVTK
  vtkQtChart
  vtkViews
  vtkInfovis
  vtklibxml2
  vtkDICOMParser
  vtkpng
  vtkzlib
  vtkjpeg
  vtkalglib
  vtkexpat
  vtkverdict
  vtkmetaio
  vtkNetCDF
  vtksqlite
  vtkexoIIc
  vtkftgl
  vtkfreetype
)

set( FVL_LIBRARIES 
  ${ITK_LIBRARIES}
  ${QT_LIBRARIES}
  ${QWT_LIBRARIES}
  ${VTK_LIBRARIES}
)

SEMMacroBuildCLI(
  NAME FiberViewerLight
  ADDITIONAL_SRCS ${FVLight_source} ${FVLight_header} ${MOC_FILES}
  TARGET_LIBRARIES ${FVL_LIBRARIES}
  EXECUTABLE_ONLY
  INCLUDE_DIRECTORIES ${QT_INCLUDE_DIR} ${FiberViewerLight_BINARY_DIR} ${FiberViewerLight_SOURCE_DIR} ${QWT_INCLUDE_DIR} ${VTK_INCLUDE_DIRS}
  )

if( EXTENSION_SUPERBUILD_BINARY_DIR )
  set( VTK_DIR_TMP ${VTK_DIR} )
  unset( VTK_DIR CACHE )
  unset( VTK_DIR )
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
  set( VTK_DIR ${VTK_DIR_TMP} CACHE PATH "VTK PATH" FORCE )
endif()

IF(BUILD_TESTING)
  include( CTest )
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)

if( EXTENSION_SUPERBUILD_BINARY_DIR )
  install(PROGRAMS ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY}/FiberViewerLight DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION})
  install(PROGRAMS ${FiberViewerLight_SOURCE_DIR}/FiberLengthCleaning.py DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION})
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
  include(${Slicer_EXTENSION_CPACK})
endif()

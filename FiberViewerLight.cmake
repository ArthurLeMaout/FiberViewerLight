
#-----------------------------------------------------------------------------
set(MODULE_NAME ${EXTENSION_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

if( NOT RUNTIME_OUTPUT_DIRECTORY )
  set( RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
  set( LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
  set( ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/static)
endif()

FIND_PACKAGE(VTK REQUIRED)
IF (VTK_FOUND)
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
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
   MESSAGE(FATAL_ERROR, "QT not found. Please set QT_DIR.")
ENDIF(QT_USE_FILE)

# Qwt include directory

if( NOT QWT_INCLUDE_DIR )
  FIND_PATH(QWT_INCLUDE_DIR qwt.h
    /usr/local/qwt/include
    /usr/local/include
    /usr/include/qwt
    /usr/include
  )
endif()


if(NOT QWT_INCLUDE_DIR)
  message(FATAL_ERROR "QWT_INCLUDE_DIR not found. It should be set to a path containing qwt.h, like '/Projects/qwt-6.0.1-svn/include'")
endif()

# Qwt libraries
if( NOT QWT_LIBRARY )
  if( EXTENSION_SUPERBUILD_BINARY_DIR )
    set(NO_ENV_PATH "NO_DEFAULT_PATH")
  endif()
  SET(QWT_NAMES ${QWT_NAMES} qwt libqwt)
  FIND_LIBRARY(QWT_LIBRARY
    NAMES ${QWT_NAMES}
    PATHS ${QWT_LIBRARY_PATH} /usr/local/qwt/lib /usr/local/lib /usr/lib
    ${NO_ENV_PATH}
  )
endif()
if(NOT QWT_LIBRARY)
  message(FATAL_ERROR "QWT_LIBRARY not found. It should be set to a filepath like '/Projects/qwt-6.0.1-svn/lib/libqwt.a'")
endif()
set(QWT_LIBRARIES ${QWT_LIBRARY})

find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE}) 

if( EXTENSION_SUPERBUILD_BINARY_DIR )
  if( APPLE )
    set( CMAKE_EXE_LINKER_FLAGS -Wl,-rpath,@loader_path/../../../../../)
  endif()
  if( WIN32 OR APPLE )
    include(${GenerateCLP_USE_FILE})
    generateclp(Launcher_CLP FiberViewerLight.xml )
    add_executable( FiberViewerLightLauncher Launcher.cxx ${Launcher_CLP} )
  endif()
endif()

QT4_WRAP_CPP(MOC_FILES FiberViewerLightGUI.h FVLengthGUI.h FVDistributionGUI.h FVPanelGUI.h FVDisplayClassGUI.h FVCutterGUI.h FVNormalizedCutGUI.h PlanSetting.h FiberDisplay.h)

SET(FVLight_source FiberViewerLight.cxx FiberViewerLightGUI.cxx FiberDisplay.cxx FVLengthGUI.cxx  FVDistributionGUI.cxx FVPanelGUI.cxx FVDisplayClassGUI.cxx FVNormalizedCutGUI.cxx PlanSetting.cxx FVCutterGUI.cxx ${MOC_FILES})
SET(FVLight_header FiberViewerLightGUI.h FiberDisplay.h FVLengthGUI.h FVDistributionGUI.h FVPanelGUI.h FVDisplayClassGUI.h FVNormalizedCutGUI.h PlanSetting.h FVCutterGUI.h)

set(VTK_LIBRARIES 
  ${VTK_LIBRARIES}
  QVTK
)

set( FVL_LIBRARIES 
  ${ITK_LIBRARIES}
  ${QT_LIBRARIES}
  ${QWT_LIBRARIES}
  ${VTK_LIBRARIES}
)

SEMMacroBuildCLI(
  NAME FiberViewerLight
  EXECUTABLE_ONLY
  ADDITIONAL_SRCS ${FVLight_source} ${FVLight_header} ${MOC_FILES}
  RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY}
  LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIRECTORY}
  ARCHIVE_OUTPUT_DIRECTORY ${ARCHIVE_OUTPUT_DIRECTORY}
  TARGET_LIBRARIES ${FVL_LIBRARIES}
  LINK_DIRECTORIES ${VTK_LIBRARY_DIRS}
  INCLUDE_DIRECTORIES ${QT_INCLUDE_DIR} ${FiberViewerLight_BINARY_DIR} ${FiberViewerLight_SOURCE_DIR} ${QWT_INCLUDE_DIR} ${VTK_INCLUDE_DIRS}
  )

if( EXTENSION_SUPERBUILD_BINARY_DIR )
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

IF(BUILD_TESTING)
  include( CTest )
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)

if( EXTENSION_SUPERBUILD_BINARY_DIR )
  if( WIN32 OR APPLE )
    install(TARGETS FiberViewerLightLauncher DESTINATION ${CLI_INSTALL_DIRECTORY})
  endif()
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
  include(${Slicer_EXTENSION_CPACK})
endif()

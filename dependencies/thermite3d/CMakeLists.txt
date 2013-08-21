CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Thermite)

#Standard modules
#FIND_PACKAGE(Boost 1.34.1 COMPONENTS program_options filesystem REQUIRED)

IF(CMAKE_COMPILER_IS_GNUCXX) #Maybe "OR MINGW"
	ADD_DEFINITIONS(-std=c++0x) #Enable C++0x mode
ENDIF()

#Custom modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
FIND_PACKAGE(OGRE 1.6 REQUIRED)
#FIND_PACKAGE(PolyVox REQUIRED)

################################################################################
## Main files
################################################################################

SET(MAIN_SRC_FILES
	source/DataStreamWrapper.cpp
	source/SurfacePatchRenderable.cpp
	source/Utility.cpp
	source/VolumeSerializationProgressListenerImpl.cpp
	
	source/AbstractSettingsWidget.cpp
	source/Application.cpp
	source/FPSDialog.cpp
	source/GraphicsSettingsWidget.cpp
	source/OgreGpuCommandBufferFlush.cpp
	source/OgreWidget.cpp
	source/Screen.cpp
	source/SettingsDialog.cpp
	source/ViewWidget.cpp
)

SET(MAIN_INC_FILES
	include/DataStreamWrapper.h
	include/QtForwardDeclarations.h
	include/SurfacePatchRenderable.h
	include/ThermiteForwardDeclarations.h
	include/Utility.h
	include/VolumeSerializationProgressListenerImpl.h
	
	include/AbstractSettingsWidget.h
	include/Application.h
	include/FPSDialog.h
	include/GraphicsSettingsWidget.h
	include/OgreGpuCommandBufferFlush.h
	include/OgreWidget.h
	include/QtOgreForwardDeclarations.h
	include/SettingsDialog.h
	include/Screen.h
	include/ViewWidget.h
)

SET(MAIN_UI_FILES
	ui/FPSDialog.ui
	ui/GraphicsSettingsWidget.ui
	ui/Log.ui
	ui/SettingsDialog.ui
)

SOURCE_GROUP("Sources" FILES ${MAIN_SRC_FILES})
SOURCE_GROUP("Headers" FILES ${MAIN_INC_FILES})
SOURCE_GROUP("UI" FILES ${MAIN_UI_FILES})

SOURCE_GROUP("Sources\\Hacks" FILES ${HACKS_SRC_FILES})
SOURCE_GROUP("Headers\\Hacks" FILES ${HACKS_INC_FILES})

################################################################################
## Logging files
################################################################################

SET(LOGGING_SRC_FILES
	source/Logging/Log.cpp
	source/Logging/LogEntry.cpp
	source/Logging/LogManager.cpp
	source/Logging/LogModel.cpp
	source/Logging/LogModelSortFilterProxy.cpp
)

SET(LOGGING_INC_FILES
	include/Logging/Log.h
	include/Logging/LogEntry.h
	include/Logging/LogLevel.h
	include/Logging/LogManager.h
	include/Logging/LogModel.h
	include/Logging/LogModelSortFilterProxy.h
)

SOURCE_GROUP("Sources\\Logging" FILES ${LOGGING_SRC_FILES})
SOURCE_GROUP("Headers\\Logging" FILES ${LOGGING_INC_FILES})

################################################################################
## Resource files
################################################################################

SET(RESOURCE_SRC_FILES
	source/Resources/QStringIODevice.cpp
	source/Resources/TextManager.cpp
	source/Resources/TextResource.cpp
	source/Resources/VolumeManager.cpp
	source/Resources/VolumeResource.cpp
)

SET(RESOURCE_INC_FILES
	include/Resources/QStringIODevice.h
	include/Resources/TextManager.h
	include/Resources/TextResource.h
	include/Resources/VolumeManager.h
	include/Resources/VolumeResource.h
)

SOURCE_GROUP("Sources\\Resources" FILES ${RESOURCE_SRC_FILES})
SOURCE_GROUP("Headers\\Resources" FILES ${RESOURCE_INC_FILES})

################################################################################
## Scripting files
################################################################################

SET(SCRIPTABLE_SRC_FILES
	source/scriptable/Camera.cpp
	source/scriptable/Component.cpp
	source/scriptable/Entity.cpp
	source/scriptable/Globals.cpp
	source/scriptable/Light.cpp
	source/scriptable/Object.cpp
	source/scriptable/RenderComponent.cpp
	source/scriptable/SkyBox.cpp
	source/scriptable/Volume.cpp
)

SET(SCRIPTABLE_INC_FILES
	include/scriptable/Camera.h
	include/scriptable/Component.h
	include/scriptable/Entity.h
	include/scriptable/Globals.h
	include/scriptable/Light.h
	include/scriptable/Object.h
	include/scriptable/RenderComponent.h
	include/scriptable/SkyBox.h
	include/scriptable/Volume.h
)

SOURCE_GROUP("Sources\\Scriptable" FILES ${SCRIPTABLE_SRC_FILES})
SOURCE_GROUP("Headers\\Scriptable" FILES ${SCRIPTABLE_INC_FILES})

################################################################################
## Tasks files
################################################################################

SET(TASKS_SRC_FILES
	source/tasks/AmbientOcclusionTask.cpp
	source/tasks/FindPathTask.cpp
	source/tasks/SurfaceMeshDecimationTask.cpp
	source/tasks/SurfaceMeshExtractionTask.cpp
	source/tasks/Task.cpp
	source/tasks/TaskProcessorThread.cpp
)

SET(TASKS_INC_FILES
	include/Tasks/AmbientOcclusionTask.h
	include/Tasks/FindPathTask.h
	include/Tasks/SurfaceMeshDecimationTask.h
	include/Tasks/SurfaceMeshExtractionTask.h
	include/Tasks/Task.h
	include/Tasks/TaskProcessorThread.h
)

SOURCE_GROUP("Sources\\Tasks" FILES ${TASKS_SRC_FILES})
SOURCE_GROUP("Headers\\Tasks" FILES ${TASKS_INC_FILES})

#Combine all the source/include/UI files into a single list
SET(SRC_FILES ${MAIN_SRC_FILES} ${LOGGING_SRC_FILES} ${RESOURCE_SRC_FILES} ${TASKS_SRC_FILES} ${SCRIPTABLE_SRC_FILES} ${HACKS_SRC_FILES})
SET(INC_FILES ${MAIN_INC_FILES} ${LOGGING_INC_FILES} ${RESOURCE_INC_FILES} ${TASKS_INC_FILES} ${SCRIPTABLE_INC_FILES} ${HACKS_INC_FILES})
SET(UI_FILES ${MAIN_UI_FILES})

SET(RESOURCE_FILES
	resources/Qt/ThermiteResources.qrc
	resources/resources.qrc
)

FIND_PACKAGE(Qt4)
IF (WIN32)
	SET(QT_USE_QTMAIN 1)
ENDIF (WIN32)
SET(QT_USE_PHONON 0)
SET(QT_USE_QTGUI 1)
SET(QT_USE_QTUITOOLS 1)
SET(QT_USE_QTXML 0)
SET(QT_USE_QTDBUS 0)
SET(QT_USE_QTSCRIPT 0)
INCLUDE(${QT_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS ${INC_FILES})
QT4_WRAP_UI(UI_SRCS ${UI_FILES})
QT4_ADD_RESOURCES(RESOURCE_SRCS ${RESOURCE_FILES})


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/Hacks ${CMAKE_CURRENT_SOURCE_DIR}/include/Logging ${CMAKE_CURRENT_SOURCE_DIR}/include/Resources ${CMAKE_CURRENT_SOURCE_DIR}/include/scriptable ${CMAKE_CURRENT_SOURCE_DIR}/include/Tasks ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include/Hacks ${CMAKE_CURRENT_BINARY_DIR}/include/Logging ${CMAKE_CURRENT_BINARY_DIR}/include/Resources ${CMAKE_CURRENT_BINARY_DIR}/include/scriptable ${CMAKE_CURRENT_BINARY_DIR}/include/Tasks ${OGRE_INCLUDE_DIRS} ${PolyVoxCore_SOURCE_DIR}/include ${PolyVoxUtil_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${OGRE_LIBRARY_DIRS} ${QtOgre_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
ADD_LIBRARY(Thermite STATIC ${SRC_FILES} ${INC_FILES} ${MOC_SRCS} ${UI_SRCS} ${RESOURCE_SRCS})
TARGET_LINK_LIBRARIES(Thermite ${QT_LIBRARIES} ${OGRE_LIBRARIES} PolyVoxCore PolyVoxUtilStatic ${BOOST_LIBRARIES})


#IF (WIN32) #Windows
	SET_TARGET_PROPERTIES(Thermite PROPERTIES COMPILE_DEFINITIONS "NOMINMAX") #Avoid Windows macros
#ENDIF (WIN32)

#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/plugins.cfg.in ${CMAKE_CURRENT_BINARY_DIR}/plugins.cfg)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/plugins_d.cfg.in ${CMAKE_CURRENT_BINARY_DIR}/plugins_d.cfg)
#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/plugins.cfg DESTINATION bin CONFIGURATIONS Release)	
#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/plugins_d.cfg DESTINATION bin CONFIGURATIONS Debug)	

#Install the resulting binaries
#INSTALL(TARGETS Thermite
#	RUNTIME DESTINATION bin
#	LIBRARY DESTINATION lib
#	ARCHIVE DESTINATION lib
#)

#IF (WIN32) #Windows
#CONFIGURE_FILE(${OGRE_LIBRARY_DIRS}/OgreMain_d.dll ${CMAKE_CURRENT_BINARY_DIR}/debug/OgreMain_d.dll COPYONLY)
#CONFIGURE_FILE(${OGRE_LIBRARY_DIRS}/RenderSystem_GL_d.dll ${CMAKE_CURRENT_BINARY_DIR}/debug/RenderSystem_GL_d.dll COPYONLY)
#CONFIGURE_FILE(${OGRE_LIBRARY_DIRS}/RenderSystem_Direct3D9_d.dll ${CMAKE_CURRENT_BINARY_DIR}/debug/RenderSystem_Direct3D9_d.dll COPYONLY)

#CONFIGURE_FILE(${OGRE_LIBRARY_DIRS}/OgreMain.dll ${CMAKE_CURRENT_BINARY_DIR}/release/OgreMain.dll COPYONLY)
#CONFIGURE_FILE(${OGRE_LIBRARY_DIRS}/RenderSystem_GL.dll ${CMAKE_CURRENT_BINARY_DIR}/release/RenderSystem_GL.dll COPYONLY)
#CONFIGURE_FILE(${OGRE_LIBRARY_DIRS}/RenderSystem_Direct3D9.dll ${CMAKE_CURRENT_BINARY_DIR}/release/RenderSystem_Direct3D9.dll COPYONLY)
#ENDIF (WIN32)

#On Windows, it is necessary to copy all the dependancies .dll's to the binary folder
#Should be able to use INCLUDE(GetPrerequisites)
IF(WIN32)
	SET(OGRESDK $ENV{OGRE_HOME})
	SET(OGRESOURCE $ENV{OGRE_SRC})
	IF (OGRESDK)
		MESSAGE(STATUS "Using stuff from OGRE SDK")
		#This first bunch of install commands is called for release mode.
		INSTALL(FILES $ENV{OGRE_HOME}/bin/release/OgreMain.dll DESTINATION bin CONFIGURATIONS Release)

		INSTALL(FILES $ENV{OGRE_HOME}/bin/release/Cg.dll DESTINATION bin CONFIGURATIONS Release)

		INSTALL(FILES $ENV{OGRE_HOME}/bin/release/RenderSystem_GL.dll DESTINATION bin CONFIGURATIONS Release)
		INSTALL(FILES $ENV{OGRE_HOME}/bin/release/RenderSystem_Direct3D9.dll DESTINATION bin CONFIGURATIONS Release)
		INSTALL(FILES $ENV{OGRE_HOME}/bin/release/Plugin_CgProgramManager.dll DESTINATION bin CONFIGURATIONS Release)
		INSTALL(FILES $ENV{OGRE_HOME}/bin/release/Plugin_ParticleFX.dll DESTINATION bin CONFIGURATIONS Release)
		
		#This second bunch of install commands is called for debug mode.
		INSTALL(FILES $ENV{OGRE_HOME}/bin/debug/OgreMain_d.dll DESTINATION bin CONFIGURATIONS Debug)

		INSTALL(FILES $ENV{OGRE_HOME}/bin/release/Cg.dll DESTINATION bin CONFIGURATIONS Debug)

		INSTALL(FILES $ENV{OGRE_HOME}/bin/debug/RenderSystem_GL_d.dll DESTINATION bin CONFIGURATIONS Debug)
		INSTALL(FILES $ENV{OGRE_HOME}/bin/debug/RenderSystem_Direct3D9_d.dll DESTINATION bin CONFIGURATIONS Debug)
		INSTALL(FILES $ENV{OGRE_HOME}/bin/debug/Plugin_CgProgramManager_d.dll DESTINATION bin CONFIGURATIONS Debug)
		INSTALL(FILES $ENV{OGRE_HOME}/bin/debug/Plugin_ParticleFX_d.dll DESTINATION bin CONFIGURATIONS Debug)
	ENDIF (OGRESDK)
	IF (OGRESOURCE)
		MESSAGE(STATUS "Using stuff from OGRE Source")
		#This first bunch of install commands is called for release mode.
		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Release/OgreMain.dll DESTINATION bin CONFIGURATIONS Release)

		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Release/Cg.dll DESTINATION bin CONFIGURATIONS Release)

		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Release/RenderSystem_GL.dll DESTINATION bin CONFIGURATIONS Release)
		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Release/RenderSystem_Direct3D9.dll DESTINATION bin CONFIGURATIONS Release)
		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Release/Plugin_CgProgramManager.dll DESTINATION bin CONFIGURATIONS Release)
		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Release/Plugin_ParticleFX.dll DESTINATION bin CONFIGURATIONS Release)

		#This second bunch of install commands is called for debug mode.
		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Debug/OgreMain_d.dll DESTINATION bin CONFIGURATIONS Debug)

		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Release/Cg.dll DESTINATION bin CONFIGURATIONS Debug)

		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Debug/RenderSystem_GL_d.dll DESTINATION bin CONFIGURATIONS Debug)
		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Debug/RenderSystem_Direct3D9_d.dll DESTINATION bin CONFIGURATIONS Debug)
		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Debug/Plugin_CgProgramManager_d.dll DESTINATION bin CONFIGURATIONS Debug)
		INSTALL(FILES $ENV{OGRE_SRC}/Samples/Common/bin/Debug/Plugin_ParticleFX_d.dll DESTINATION bin CONFIGURATIONS Debug)
	ENDIF (OGRESOURCE)
	
	#Install the PolyVox .dll's
	INSTALL(FILES ${PolyVoxCore_INCLUDE_DIRS}/../bin/PolyVoxCore.dll DESTINATION bin)
	INSTALL(FILES ${PolyVoxUtil_INCLUDE_DIRS}/../bin/PolyVoxUtil.dll DESTINATION bin)
	
	#And the debug files
	INSTALL(FILES ${PolyVoxCore_INCLUDE_DIRS}/../lib/PolyVoxCore.pdb DESTINATION bin CONFIGURATIONS Debug)
	INSTALL(FILES ${PolyVoxUtil_INCLUDE_DIRS}/../lib/PolyVoxUtil.pdb DESTINATION bin CONFIGURATIONS Debug)
	
	#Install the Qt .dll's
	INSTALL(FILES ${QT_BINARY_DIR}/QtCore4.dll DESTINATION bin CONFIGURATIONS Release)
	INSTALL(FILES ${QT_BINARY_DIR}/QtGui4.dll DESTINATION bin CONFIGURATIONS Release)
	INSTALL(FILES ${QT_BINARY_DIR}/QtSvg4.dll DESTINATION bin CONFIGURATIONS Release)
	INSTALL(FILES ${QT_BINARY_DIR}/QtXml4.dll DESTINATION bin CONFIGURATIONS Release)
	INSTALL(FILES ${QT_PLUGINS_DIR}/imageformats/qmng4.dll DESTINATION bin/qt-plugins/imageformats CONFIGURATIONS Release)
	INSTALL(FILES ${QT_PLUGINS_DIR}/imageformats/qsvg4.dll DESTINATION bin/qt-plugins/imageformats CONFIGURATIONS Release)
	
	INSTALL(FILES ${QT_BINARY_DIR}/QtCored4.dll DESTINATION bin CONFIGURATIONS Debug)
	INSTALL(FILES ${QT_BINARY_DIR}/QtGuid4.dll DESTINATION bin CONFIGURATIONS Debug)
	INSTALL(FILES ${QT_BINARY_DIR}/QtSvgd4.dll DESTINATION bin CONFIGURATIONS Debug)
	INSTALL(FILES ${QT_BINARY_DIR}/QtXmld4.dll DESTINATION bin CONFIGURATIONS Debug)
	INSTALL(FILES ${QT_PLUGINS_DIR}/imageformats/qmngd4.dll DESTINATION bin/qt-plugins/imageformats CONFIGURATIONS Debug)
	INSTALL(FILES ${QT_PLUGINS_DIR}/imageformats/qsvgd4.dll DESTINATION bin/qt-plugins/imageformats CONFIGURATIONS Debug)
	
	
	#Install the qt configuration file
	INSTALL(FILES qt.conf DESTINATION bin CONFIGURATIONS Release)	
	INSTALL(FILES qt.conf DESTINATION bin CONFIGURATIONS Debug)
	
	#Install the settings file
	INSTALL(FILES Settings.ini DESTINATION bin CONFIGURATIONS Release)	
	INSTALL(FILES Settings.ini DESTINATION bin CONFIGURATIONS Debug)	
	
ENDIF(WIN32)

#Lastly, we copy the core resources and set up the 'share' folder
#INSTALL(DIRECTORY resources/Ogre/debug DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/resources/debug PATTERN "*.svn*" EXCLUDE) #this will exclude the .svn files
#INSTALL(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/share/thermite/apps)

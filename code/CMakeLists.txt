CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(TankWars)

#Standard modules
FIND_PACKAGE(Boost 1.34.1 COMPONENTS program_options filesystem system REQUIRED)

IF(CMAKE_COMPILER_IS_GNUCXX) #Maybe "OR MINGW"
	ADD_DEFINITIONS(-std=c++0x) #Enable C++0x mode
ENDIF()

#Custom modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
FIND_PACKAGE(OGRE 1.6 REQUIRED)
#FIND_PACKAGE(PolyVox REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)

SET(SRC_FILES
	source/Config.cpp
	source/FixedShadowCameraSetup.cpp
	source/GameScreen.cpp
	source/HighScoreScreen.cpp
	source/KeyBindingDialog.cpp
	source/Keyboard.cpp
	source/main.cpp
	source/MainMenuScreen.cpp
	source/Mouse.cpp
	source/MusicPlayer.cpp
	source/PausedScreen.cpp
	source/PlayScreen.cpp
	source/SettingsDialog.cpp
	source/TankWarsApplication.cpp
	source/TankWarsViewWidget.cpp
	#source/TerrainGeneration.cpp
	source/Text3D.cpp
)

SET(INC_FILES
	include/Config.h
	include/FixedShadowCameraSetup.h
	include/GameScreen.h
	include/HighScoreScreen.h
	include/KeyBindingDialog.h
	include/Keyboard.h
	include/MainMenuScreen.h
	include/Mouse.h
	include/MusicPlayer.h
	include/PausedScreen.h
	include/PlayScreen.h
	include/SettingsDialog.h
	include/TankWarsApplication.h
	include/TankWarsViewWidget.h
	#include/TerrainGeneration.h
	include/Text3D.h
)

SET(UI_FILES
	ui/SettingsDialog.ui
	ui/KeyBindingDialog.ui
)

SOURCE_GROUP("Sources" FILES ${SRC_FILES})
SOURCE_GROUP("Headers" FILES ${INC_FILES})
SOURCE_GROUP("UI" FILES ${UI_FILES})

FIND_PACKAGE(Qt4)
IF (WIN32)
	SET(QT_USE_QTMAIN 1)
ENDIF (WIN32)
SET(QT_USE_QTGUI 1)
SET(QT_USE_QTUITOOLS 1)
INCLUDE(${QT_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS ${INC_FILES})
QT4_WRAP_UI(UI_SRCS ${UI_FILES})
QT4_ADD_RESOURCES(RESOURCE_SRCS ${RESOURCE_FILES})

SET(THERMITE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/thermite3d/)
#SET(THERMITE_PATH /home/matt/thermite3d/Thermite)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include ${THERMITE_PATH}/include ${THERMITE_PATH}/include/Logging ${THERMITE_PATH}/include/Resources ${THERMITE_PATH}/include/scriptable ${THERMITE_PATH}/include/Tasks ${CMAKE_CURRENT_BINARY_DIR} ${THERMITE_PATH}/include ${THERMITE_PATH}/include/Logging ${THERMITE_PATH}/include/Resources ${THERMITE_PATH}/include/scriptable ${THERMITE_PATH}/include/Tasks ${OGRE_INCLUDE_DIRS} ${PolyVoxCore_SOURCE_DIR}/include ${PolyVoxUtil_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS} ${SDL_INCLUDE_DIRS} ${SDL_MIXER_INCLUDE_DIRS} ${QT_QTCORE_INCLUDE_DIR} ${QT_INCLUDES})
LINK_DIRECTORIES(${OGRE_LIBRARY_DIRS} ${QtOgre_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS} ${THERMITE_PATH}/build)
ADD_EXECUTABLE(Voxeliens WIN32 ${SRC_FILES} ${INC_FILES} ${MOC_SRCS} ${UI_SRCS} ${RESOURCE_SRCS} icon.rc)
MESSAGE(STATUS ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(Voxeliens ${QT_LIBRARIES} ${OGRE_LIBRARIES} PolyVoxCore ${Boost_LIBRARIES} Thermite ${SDL_LIBRARY} ${SDL_MIXER_LIBRARIES})
IF(UNIX)
	FIND_PACKAGE(X11 REQUIRED)
	INCLUDE_DIRECTORIES(${X11_X11_INCLUDE_PATH})
	TARGET_LINK_LIBRARIES(Voxeliens ${X11_X11_LIB})
ENDIF()

install(TARGETS Voxeliens RUNTIME DESTINATION bin)
install(DIRECTORY ../audio DESTINATION resources)
install(DIRECTORY ../materials DESTINATION resources)
install(DIRECTORY ../models DESTINATION resources)
install(DIRECTORY ../volumes DESTINATION resources)
install(DIRECTORY ../voxels DESTINATION resources)

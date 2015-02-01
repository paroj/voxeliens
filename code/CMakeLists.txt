CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(TankWars)

IF(CMAKE_COMPILER_IS_GNUCXX) #Maybe "OR MINGW"
	ADD_DEFINITIONS(-std=c++0x) #Enable C++0x mode
ENDIF()

#Custom modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
FIND_PACKAGE(OGRE 1.9 REQUIRED)
#FIND_PACKAGE(PolyVox REQUIRED)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED SDL2)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)

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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include ${THERMITE_PATH}/include ${THERMITE_PATH}/include/Logging ${THERMITE_PATH}/include/Resources ${THERMITE_PATH}/include/scriptable ${THERMITE_PATH}/include/Tasks ${CMAKE_CURRENT_BINARY_DIR} ${THERMITE_PATH}/include ${THERMITE_PATH}/include/Logging ${THERMITE_PATH}/include/Resources ${THERMITE_PATH}/include/scriptable ${THERMITE_PATH}/include/Tasks ${OGRE_INCLUDE_DIRS} ${PolyVoxCore_SOURCE_DIR}/include ${PolyVoxUtil_SOURCE_DIR}/include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${QT_QTCORE_INCLUDE_DIR} ${QT_INCLUDES})
LINK_DIRECTORIES(${OGRE_LIBRARY_DIRS} ${QtOgre_LIBRARY_DIRS} ${THERMITE_PATH}/build)
ADD_EXECUTABLE(Voxeliens WIN32 ${SRC_FILES} ${INC_FILES} ${MOC_SRCS} ${UI_SRCS} ${RESOURCE_SRCS} icon.rc)
#MESSAGE(STATUS ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(Voxeliens ${QT_LIBRARIES} ${OGRE_LIBRARIES} PolyVoxCore Thermite ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} SDL2_mixer boost_system)
IF(UNIX)
	FIND_PACKAGE(X11 REQUIRED)
	INCLUDE_DIRECTORIES(${X11_X11_INCLUDE_PATH})
	TARGET_LINK_LIBRARIES(Voxeliens ${X11_X11_LIB})
ENDIF()

install(TARGETS Voxeliens RUNTIME DESTINATION bin) #Should this be 'games' instead?	
install(DIRECTORY ../audio DESTINATION share/games/voxeliens)
install(DIRECTORY ../materials DESTINATION share/games/voxeliens)
install(DIRECTORY ../models DESTINATION share/games/voxeliens)
install(DIRECTORY ../volumes DESTINATION share/games/voxeliens)
install(DIRECTORY ../voxels DESTINATION share/games/voxeliens)

configure_file(../voxeliens.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/voxeliens.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/voxeliens.desktop DESTINATION share/applications)

install(FILES ../128.png DESTINATION share/icons/hicolor/128x128/apps RENAME voxeliens.png)
install(FILES ../64.png DESTINATION share/icons/hicolor/64x64/apps RENAME voxeliens.png)
install(FILES ../48.png DESTINATION share/icons/hicolor/48x48/apps RENAME voxeliens.png)
install(FILES ../24.png DESTINATION share/icons/hicolor/24x24/apps RENAME voxeliens.png)
install(FILES ../22.png DESTINATION share/icons/hicolor/22x22/apps RENAME voxeliens.png)
install(FILES ../16.png DESTINATION share/icons/hicolor/16x16/apps RENAME voxeliens.png)

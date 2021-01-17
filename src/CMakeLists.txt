project( QtGreet )
cmake_minimum_required( VERSION 3.8 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_BUILD_TYPE Release )

add_definitions ( -Wall )
if ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Location for installing the project" FORCE )
endif()

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )
cmake_policy( SET CMP0071 NEW )

find_package ( Qt5WaylandClient REQUIRED )
find_package ( Qt5Widgets REQUIRED )
find_package ( Qt5Core REQUIRED )
find_package ( Qt5Gui REQUIRED )
find_package ( Qt5DBus REQUIRED )

find_package(ECM REQUIRED)
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_FIND_MODULE_DIR} )

find_package( WaylandScanner REQUIRED )
find_package( PkgConfig )
pkg_check_modules( wayland-client REQUIRED wayland-client )

set ( HDRS
	Global.hpp
	ClockWidget.hpp
	QtGreet.hpp
	proto.h
)

set ( SRCS
	proto.c
	ClockWidget.cpp
	QtGreet.cpp
	Main.cpp
)

set ( RSCS
	QtGreet.qrc
)

execute_process(COMMAND pkg-config wayland-protocols --variable=pkgdatadir OUTPUT_VARIABLE WAYLAND_PROTOCOLS_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

ECM_ADD_WAYLAND_CLIENT_PROTOCOL( WL_PROT_SRC PROTOCOL protocols/xdg-shell.xml BASENAME xdg-shell )
ECM_ADD_WAYLAND_CLIENT_PROTOCOL( WL_PROT_SRC PROTOCOL protocols/wlr-layer-shell-unstable-v1.xml BASENAME wlr-layer-shell-unstable-v1 )

add_executable( qtgreet ${SRCS} ${HDRS} ${RSCS} ${WL_PROT_SRC} )
target_include_directories( qtgreet PRIVATE protocols )
target_link_libraries ( qtgreet Qt5::Core Qt5::Gui Qt5::GuiPrivate Qt5::Widgets Qt5::DBus ${wayland-client_LIBRARIES} json-c )

# Main Executable
install( TARGETS qtgreet DESTINATION bin )

# SVG Backgrounds
install( DIRECTORY ../backgrounds DESTINATION share/qtgreet )

# Configuration
install( FILES ../configs/config.ini DESTINATION /etc/qtgreet )
install( FILES ../configs/wayfire.ini DESTINATION /etc/qtgreet )

# Data files
install( FILES ../README.md ../Changelog ../ReleaseNotes DESTINATION share/qtgreet )

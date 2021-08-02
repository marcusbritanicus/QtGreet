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

find_package( PkgConfig )

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

add_executable( qtgreet ${SRCS} ${HDRS} ${RSCS} )
target_link_libraries ( qtgreet Qt5::Core Qt5::Gui Qt5::GuiPrivate Qt5::Widgets Qt5::DBus json-c )

# Main Executable
install( TARGETS qtgreet DESTINATION bin )

# SVG Backgrounds
install( DIRECTORY ../backgrounds DESTINATION share/qtgreet )

# Configuration
install( FILES ../configs/config.ini DESTINATION /etc/qtgreet )
install( FILES ../configs/wayfire.ini DESTINATION /etc/qtgreet )

# Data files
install( FILES ../README.md ../Changelog ../ReleaseNotes DESTINATION share/qtgreet )

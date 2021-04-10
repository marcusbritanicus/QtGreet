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

include_directories(
    src
    src/backend
    src/layoutmanager
    src/layoutmanager/parser
    src/thememanager
    src/ui
    src/widgetfactory
    src/widgets
    src/widgets/clock
)

set ( HDRS
	src/Global.hpp
    src/layoutmanager/parser/hjson.h
    src/layoutmanager/LayoutManager.hpp
    src/ui/QtGreet.hpp
    src/widgetfactory/WidgetFactory.hpp
    src/widgets/clock/ClockWidget.hpp
)

set ( SRCS
    src/layoutmanager/LayoutManager.cpp
    src/layoutmanager/parser/hjson_decode.cpp
    src/layoutmanager/parser/hjson_encode.cpp
    src/layoutmanager/parser/hjson_parsenumber.cpp
    src/layoutmanager/parser/hjson_value.cpp
    src/Main.cpp
    src/ui/QtGreet.cpp
    src/widgetfactory/WidgetFactory.cpp
    src/widgets/clock/ClockWidget.cpp
)

set ( RSCS
	src/QtGreet.qrc
)

add_executable( qtgreet ${SRCS} ${HDRS} ${RSCS} )
target_link_libraries ( qtgreet Qt5::Core Qt5::Gui Qt5::GuiPrivate Qt5::Widgets Qt5::DBus )

# Main Executable
install( TARGETS qtgreet DESTINATION bin )

# SVG Backgrounds
install( DIRECTORY backgrounds DESTINATION share/qtgreet )

# Configuration
install( FILES configs/config.ini DESTINATION /etc/qtgreet )
install( FILES configs/wayfire.ini DESTINATION /etc/qtgreet )

# Data files
install( FILES README.md Changelog ReleaseNotes DESTINATION share/qtgreet )

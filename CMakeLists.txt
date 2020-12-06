project( QtGreet )
cmake_minimum_required( VERSION 3.1 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_BUILD_TYPE Release )

add_definitions ( -Wall )
set( CMAKE_INSTALL_PREFIX /usr )

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
)

set ( SRCS
	QtGreet.cpp
	Main.cpp
)

set ( HMOCS
	QtGreet.hpp
)
QT5_WRAP_CPP(MOCS ${HMOCS})

# set ( RSRCS
	# DropDown.qrc
# )
# QT5_ADD_RESOURCES(RSCS ${RSRCS})

execute_process(COMMAND pkg-config wayland-protocols --variable=pkgdatadir OUTPUT_VARIABLE WAYLAND_PROTOCOLS_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

ECM_ADD_WAYLAND_CLIENT_PROTOCOL( WL_PROT_SRC PROTOCOL protocols/xdg-shell.xml BASENAME xdg-shell )
ECM_ADD_WAYLAND_CLIENT_PROTOCOL( WL_PROT_SRC PROTOCOL protocols/wlr-layer-shell-unstable-v1.xml BASENAME wlr-layer-shell-unstable-v1 )

add_executable ( qtgreet ${SRCS} ${RSCS} ${MOCS} ${WL_PROT_SRC} )
target_include_directories( qtgreet PRIVATE protocols )
target_link_libraries ( qtgreet Qt5::Core Qt5::Gui Qt5::GuiPrivate Qt5::Widgets Qt5::DBus ${wayland-client_LIBRARIES} )

# Main Executable
install( TARGETS qtgreet DESTINATION bin )

# # 512px png
# install( FILES resources/desq-dropdown.png DESTINATION /usr/share/icons/hicolor/256x256/apps/ )
#
# # SVG Icon
# install( FILES resources/desq-dropdown.svg DESTINATION /usr/share/icons/hicolor/scalable/apps/ )
#
# # Data files
# install( FILES README.md LICENSE DESTINATION share/qtgreet/ )

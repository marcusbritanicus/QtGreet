project( LytParser )

cmake_minimum_required(VERSION 3.10)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_BUILD_TYPE Release )

add_definitions ( -Wall )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )
cmake_policy( SET CMP0071 NEW )

find_package ( Qt5Widgets REQUIRED )
find_package ( Qt5Core REQUIRED )
find_package ( Qt5Gui REQUIRED )

include_directories(
	parser
	../widgetfactory
)

set ( SRCS
	../widgetfactory/WidgetFactory.cpp
	../widgetfactory/WidgetFactory.hpp
	parser/hjson_decode.cpp
	parser/hjson_encode.cpp
	parser/hjson_parsenumber.cpp
	parser/hjson_value.cpp
	LayoutManager.cpp
	LayoutManager.hpp
    main.cpp
)

add_executable( parser ${SRCS} )
target_link_libraries( parser Qt5::Core Qt5::Gui Qt5::Widgets )

project( LytParser )

cmake_minimum_required(VERSION 3.10)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_BUILD_TYPE Release )

add_definitions ( -Wall )

include_directories( parser )

set ( SRCS
	parser/hjson_decode.cpp
	parser/hjson_encode.cpp
	parser/hjson_parsenumber.cpp
	parser/hjson_value.cpp
    main.cpp
)

add_executable( parser ${SRCS} )

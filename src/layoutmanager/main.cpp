#include <iostream>
#include "hjson.h"

void printObjectNames( Hjson::Value lyt, std::string space, bool vertical ) {

    for( size_t i = 0; i < lyt[ "Objects" ].size(); i++ ) {
        std::string obj = Hjson::Value( lyt[ "Objects" ][ i ] ).key( 0 );
        if ( obj == "VerticalLayout" )
            printObjectNames( lyt[ "Objects" ][ i ][ obj ], space + "    ", true );

        else if ( obj == "HorizontalLayout" )
            printObjectNames( lyt[ "Objects" ][ i ][ obj ], space + "    ", false );

        else if ( obj == "QWidget" )
            // If the widget's layout is vertical
            if ( lyt[ "Objects" ][ i ][ obj ][ "VerticalLayout" ].type() != Hjson::Type::Undefined )
                printObjectNames( lyt[ "Objects" ][ i ][ obj ][ "VerticalLayout" ], space + "    ", true );

            // If the widget's layout is horizontal
            else if ( lyt[ "Objects" ][ i ][ obj ][ "HorizontalLayout" ].type() != Hjson::Type::Undefined )
                printObjectNames( lyt[ "Objects" ][ i ][ obj ][ "HorizontalLayout" ], space + "    ", false );

            // Widget does not have a layout
            else
                std::cout << space << obj << ( vertical ? "\n" : "    " );

        else
            std::cout << space << obj << ( vertical ? "\n" : "    " );
    }

    std::cout << ( vertical ? "" : "\n" );
}

int main( int argc, char *argv[] ) {

    Hjson::Value lyt = Hjson::UnmarshalFromFile( argv[ 1 ] );

    printObjectNames( lyt[ "VerticalLayout" ], "", true );

    return 0;
}

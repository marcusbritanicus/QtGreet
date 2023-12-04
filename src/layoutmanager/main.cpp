#include <iostream>
#include "LayoutManager.hpp"

int main( int argc, char *argv[] ) {
    QApplication app( argc, argv );

    QtGreet::Layout *lyt = new QtGreet::Layout();

    lyt->generateLayout( argv[ 1 ] );

    return 0;
}

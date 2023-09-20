/**
 * Copyright 2020-2022 Britanicus <marcusbritanicus@gmail.com>
 * This file is a part of QtGreet project (https://gitlab.com/marcusbritanicus/QtGreet)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 **/

#include "QtGreet.hpp"
#include "ScreenManager.hpp"

#include <unistd.h>

QtGreet::ScreenManager::ScreenManager() {
    /** All the screens are currently "available", since none of them have been used */
    QList<QScreen *> availScrns = qApp->screens();

    /** Let's now start creating instances of QtGreet for each of the available screens */
    for ( QScreen *scrn: availScrns ) {
        QtGreet::UI *ui = new QtGreet::UI( scrn );

        /** Map the screento the greeter */
        screenGreeterMap[ scrn ] = ui;

        /** Map the UI to the screen */
        greeterScreenMap[ ui->windowTitle() ] = scrn;
    }

    /** Create a new instance when a screen is added */
    connect(
        qApp, &QApplication::screenAdded, [ = ] ( QScreen *scrn ) mutable {
            QtGreet::UI *ui = new QtGreet::UI( scrn );

            /** Map the screento the greeter */
            screenGreeterMap[ scrn ] = ui;

            /** Map the UI to the screen */
            greeterScreenMap[ ui->windowTitle() ] = scrn;

            /**  Show the greeter */
            ui->showFullScreen();

            /** Wait for Qt to process the pending events */
            qApp->processEvents();

            /** Sleep for 10 ms */
            usleep( 10 * 1000 );
        }
    );

    /** Remove the existing instance when a screen is removed */
    connect(
        qApp, &QApplication::screenRemoved, [ = ] ( QScreen *scrn ) mutable {
            QString instance = QString( "QtGreet-%1-%2x%3" ).arg( scrn->name() ).arg( scrn->size().width() ).arg( scrn->size().height() );

            greeterScreenMap.remove( instance );
            QtGreet::UI *ui = screenGreeterMap.take( scrn );
            ui->close();

            delete ui;
        }
    );
}


void QtGreet::ScreenManager::showInstacnes() {
    /** Show each of the greeters, force Qt to process the event queue, sleep of 10 ms */
    for ( QtGreet::UI *ui: screenGreeterMap.values() ) {
        /**  Show the greeter */
        ui->showFullScreen();

        /** Wait for Qt to process the pending events */
        qApp->processEvents();

        /** Sleep for 10 ms */
        usleep( 10 * 1000 );
    }
}


QScreen * QtGreet::ScreenManager::screenForWindow( QString title ) {
    if ( greeterScreenMap.contains( title ) ) {
        return greeterScreenMap[ title ];
    }

    return nullptr;
}

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

#include <QtWidgets>

#include "power.hpp"

PowerButton::PowerButton( QString type ) : QToolButton() {
    setAutoRaise( true );
    setFixedSize( QSize( 40, 40 ) );
    setIconSize( QSize( 32, 32 ) );

    if ( type == "Menu" ) {
        setIcon( QIcon( ":/icons/shutdown.png" ) );

        setToolTip( "Logout Menu" );

        QMenu *menu = new QMenu();
        menu->addAction( QIcon( ":/icons/suspend.png" ),   "Suspend to RAM" );
        menu->addAction( QIcon( ":/icons/hibernate.png" ), "Suspend to Disk" );
        menu->addAction( QIcon( ":/icons/shutdown.png" ),  "Halt System" );
        menu->addAction( QIcon( ":/icons/reboot.png" ),    "Reboot System" );

        setMenu( menu );
        setPopupMode( QToolButton::InstantPopup );
    }

    else if ( type == "Halt" ) {
        setObjectName( "Halt" );
        setToolTip( "Shutdown the system" );
        setIcon( QIcon( ":/icons/shutdown.png" ) );
    }

    else if ( type == "Reboot" ) {
        setObjectName( "Reboot" );
        setToolTip( "Reboot the system" );
        setIcon( QIcon( ":/icons/reboot.png" ) );
    }

    else if ( type == "Suspend" ) {
        setObjectName( "Suspend" );
        setToolTip( "Suspend to RAM" );
        setIcon( QIcon( ":/icons/suspend.png" ) );
    }

    else if ( type == "Hibernate" ) {
        setObjectName( "Hibernate" );
        setToolTip( "Suspend to Disk" );
        setIcon( QIcon( ":/icons/hibernate.png" ) );
    }

    else if ( type == "Logout" ) {
        setObjectName( "Logout" );
        setToolTip( "Logout from the current session" );
        setIcon( QIcon( ":/icons/logout.png" ) );
    }
}

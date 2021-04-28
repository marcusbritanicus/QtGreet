/*
	*
	* Copyright 2020 Britanicus <marcusbritanicus@gmail.com>
	*
	* This file is a part of QtGreet project
	* (https://gitlab.com/marcusbritanicus/QtGreet/)
	* Qt5 example of analog clock is the inspiration for this.
	* Some parts of the code are taken from Qt5's example.
	*

	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 3 of the License, or
	* (at your option) any later version.
	*

	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.
	*

	*
	* You should have received a copy of the GNU General Public License
	* along with this program; if not, write to the Free Software
	* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	* MA 02110-1301, USA.
	*
*/

#include <QtWidgets>

#include "buttons.hpp"

NavButton::NavButton( QString name, QString type ) : QToolButton() {

    setObjectName( name + type );
    setFixedSize( QSize( 27, 27 ) );
    setIconSize( QSize( 22, 22 ) );
    setAutoRaise( true );

    if ( type == "Right" )
        setIcon( QIcon( ":/icons/arrow-right.png" ) );

    else
        setIcon( QIcon( ":/icons/arrow-left.png" ) );
};

SessionEditButton::SessionEditButton() : QToolButton() {

    setObjectName( "SessionEditButton" );
    setFixedSize( QSize( 27, 27 ) );
    setIconSize( QSize( 22, 22 ) );
    setAutoRaise( true );

    setIcon( QIcon( ":/icons/edit.png" ) );
};

LoginToolButton::LoginToolButton() : QToolButton() {

    setObjectName( "LoginButton" );
    setFixedSize( QSize( 27, 27 ) );
    setIconSize( QSize( 22, 22 ) );
    setAutoRaise( true );

    setIcon( QIcon( ":/icons/arrow-right.png" ) );
};

LoginPushButton::LoginPushButton() : QPushButton() {

    setObjectName( "LoginButton" );
    setFixedSize( QSize( 27, 27 ) );
    setIconSize( QSize( 22, 22 ) );
    setFlat( true );

    setText( "&Login" );
    setIcon( QIcon( ":/icons/arrow-right.png" ) );
};

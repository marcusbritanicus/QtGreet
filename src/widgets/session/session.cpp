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

#include <unistd.h>
#include <QtWidgets>

#include "session.hpp"

static bool IsExec( QString exec ) {

    /* If this is a full path (ex: /usr/bin/fish) */
    if ( access( exec.toUtf8().constData(), R_OK | X_OK ) == 0 )
        return true;

    /* Otherwise (ex: plasmashell) */
    for( QString path : qgetenv( "PATH" ).split( ':' ) ) {
        int ret = access( QString( path + "/" + exec ).toUtf8().constData(), R_OK | X_OK );
        if ( ret == 0 )
            return true;
    }

    return false;
};

SessionEdit::SessionEdit() : QLineEdit() {

    setObjectName( "SessionEdit" );
    setMinimumSize( QSize( 200, 27 ) );
};

SessionName::SessionName() : QPushButton() {

    setObjectName( "SessionName" );
    setFixedHeight( 27 );
    setIcon( QIcon( ":/icons/session.png" ) );
    setText( "DesQ (Wayland)" );
    setFlat( true );
};

SessionList::SessionList() : QComboBox() {

    setObjectName( "SessionList" );

    mSessions.clear();

    QDir wlSessDir( "/usr/share/wayland-sessions" );
    for( QString sess: wlSessDir.entryList( { "*.desktop" } ) ) {
        QSettings session( wlSessDir.filePath( sess ), QSettings::IniFormat );
        if ( IsExec( session.value( "Desktop Entry/TryExec" ).toString() ) ) {
            Session s = Session{
                session.value( "Desktop Entry/Name" ).toString(),
                session.value( "Desktop Entry/Icon", ":/icons/session.png" ).toString(),
                "wayland",
                session.value( "Desktop Entry/Exec" ).toString(),
                wlSessDir.filePath( sess )
            };

            if ( not s.name.contains( "wayland", Qt::CaseInsensitive ) )
                s.name += " (Wayland)";

            mSessions << s;
        }
    }

    QDir xSessDir( "/usr/share/xsessions" );
    for( QString sess: xSessDir.entryList( { "*.desktop" } ) ) {
        QSettings session( xSessDir.filePath( sess ), QSettings::IniFormat );
        if ( IsExec( session.value( "Desktop Entry/TryExec" ).toString() ) ) {
            Session s = Session{
                session.value( "Desktop Entry/Name" ).toString() + " (X11)",
                session.value( "Desktop Entry/Icon", ":/icons/session.png" ).toString(),
                "X11",
                session.value( "Desktop Entry/Exec" ).toString(),
                xSessDir.filePath( sess )
            };

            mSessions << s;
        }
    }

    Session custom{ "Custom", "application-x-executable", "unknown", "", "" };
    mSessions << custom;

    for( Session sess: mSessions ) {
        addItem( QIcon::fromTheme( sess.icon, QIcon( sess.icon ) ), sess.name, QVariant::fromValue( sess ) );
    }
};

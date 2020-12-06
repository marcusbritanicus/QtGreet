/*
	*
	* Copyright 2019 Britanicus <marcusbritanicus@gmail.com>
	*
	* This file is a part of QtGreet project (https://gitlab.com/marcusbritanicus/qtgreet)
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

#include "QtGreet.hpp"

static bool IsExec( QString exec ) {

    /* If this is a full path (ex: /usr/bin/fish) */
    if ( access( exec.toUtf8().constData(), R_OK | X_OK ) == 0 )
        return true;

    /* Otherwise (ex: plasmashell) */
    for( QString path : qgetenv( "PATH" ).split( ':' ) ) {
        int ret = access( QString( path + "/" + exec ).toUtf8().constData(), R_OK | X_OK );
        return ( ret == 0 );
    }

    return false;
};

QtGreet::QtGreet() {

    setFixedSize( qApp->primaryScreen()->size() );

    getUserList();
    getLoginSessions();
};

/* List the users on this system */
void QtGreet::getUserList() {
    // Read /etc/passwd and list the ones with UID > login.defs[UID_MIN]

    /* Read /etc/login.defs */
    QFile loginDefs( "/etc/login.defs" );
    loginDefs.open( QFile::ReadOnly );
    QString defs = QString::fromLocal8Bit( loginDefs.readAll() );
    loginDefs.close();

    int uid_min = 1000;
    int uid_max = 60000;

    for( QString def: defs.split( "\n", QString::SkipEmptyParts ) ) {
        if ( def.startsWith( "UID_MIN" ) )
            uid_min = def.simplified().split( " ", QString::SkipEmptyParts ).value( 1 ).toInt();

        if ( def.startsWith( "UID_MAX" ) )
            uid_max = def.simplified().split( " ", QString::SkipEmptyParts ).value( 1 ).toInt();
    }

    /* Read and parse /etc/passwd */
    QFile passwd( "/etc/passwd" );
    passwd.open( QFile::ReadOnly );
    QString usrData = QString::fromLocal8Bit( passwd.readAll() );
    passwd.close();

    mUserList.clear();

    for( QString user: usrData.split( "\n", QString::SkipEmptyParts ) ) {
        QStringList uInfo = user.split( ":", QString::KeepEmptyParts );
        if ( ( uInfo.value( 2 ).toInt() >= uid_min ) and ( uInfo.value( 3 ).toInt() <= uid_max ) ) {
            User usr = User{
                uInfo.value( 0 ),
                uInfo.value( 2 ).toUInt(),
                uInfo.value( 3 ).toUInt(),
                uInfo.value( 4 ),
                uInfo.value( 5 ),
                uInfo.value( 6 )
            };

            mUserList << usr;
        }
    }
};

/* List the Wayland/X11 Sessions */
void QtGreet::getLoginSessions() {

    mSessions.clear();

    QDir wlSessDir( "/usr/share/wayland-sessions" );
    for( QString sess: wlSessDir.entryList( { "*.desktop" } ) ) {
        QSettings session( wlSessDir.filePath( sess ) );
        if ( IsExec( session.value( "Desktop Entry/TryExec" ).toString() ) ) {
            Session s = Session{
                session.value( "Desktop Entry/Name" ).toString(),
                session.value( "Desktop Entry/Icon" ).toString(),
                "wayland",
                session.value( "Desktop Entry/Exec" ).toString()
            };
        }
    }

    QDir xSessDir( "/usr/share/xsessions" );
    for( QString sess: xSessDir.entryList( { "*.desktop" } ) ) {
        QSettings session( xSessDir.filePath( sess ) );
        if ( IsExec( session.value( "Desktop Entry/TryExec" ).toString() ) ) {
            Session s = Session{
                session.value( "Desktop Entry/Name" ).toString(),
                session.value( "Desktop Entry/Icon" ).toString(),
                "X11",
                session.value( "Desktop Entry/Exec" ).toString()
            };
        }
    }
};

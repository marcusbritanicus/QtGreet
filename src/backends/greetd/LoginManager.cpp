/*
	*
	* Copyright 2020 Britanicus <marcusbritanicus@gmail.com>
	*
	* This file is a part of QtGreet project (https://gitlab.com/marcusbritanicus/QtGreet)
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

#include "LoginManager.hpp"
extern "C" {
    #include "proto.h"
}

LoginManager::LoginManager() {

	/** Nothing much */
};

bool LoginManager::authenticate( QString username, QString password ) {

	/** Let's ask greetd to create a session */
	request req_create_sess = {
        .request_type = request_type_create_session,
    };

    /** Add the username  */
    strncpy( req_create_sess.body.request_create_session.username, username.toUtf8().constData(), 127 );

    /** Send this request */
    response resp = roundtrip( req_create_sess );

	/** Request was successful. GreetD wants to authenticate */
	if ( resp.response_type == response_type_auth_message ) {

		/** We're now posting a auth message with password */
		request req = {
            .request_type = request_type_post_auth_message_response,
        };

		/** Add the password */
        strncpy( req.body.request_post_auth_message_response.response, password.toUtf8().constData(), 127 );

		/** Send the request */
        resp = roundtrip( req );

		/* Authentication should be successful; anything else will be considered a failure */
        if ( resp.response_type == response_type_success )
			return true;
	}

	return false;
};

bool LoginManager::startSession( QString baseCmd, QString type ) {

	/** We now request greetd to start the selected session */
	request req = {
		.request_type = request_type_start_session,
	};

	QString cmd;
	QString logName = "/tmp/QtGreet-" + QDateTime::currentDateTime().toString( "ddMMyyyy-hhmmss" ) + ".log";

	if ( type == "wayland" )
		cmd = baseCmd + " > " + logName + " 2>&1";

	else if ( type == "X11" )
		cmd = getX11Session( baseCmd ) + " > " + logName + " 2>&1";

	else
		cmd = baseCmd;

	/** Add the command to the request */
	strncpy( req.body.request_start_session.cmd, cmd.toUtf8().constData(), 256 );

	/** Send the request */
	response resp = roundtrip( req );

	if ( resp.response_type == response_type_success )
		return true;

	return false;
};

QString LoginManager::getX11Session( QString base ) {

	QString xinit( "xinit %1 -- /etc/X11/xinit/xserverrc :%2 vt%3 -keeptty -noreset -novtswitch -auth /tmp/Xauth.%4" );

    /* Arg2: Get the display */
    int display;
    for( display = 0; display < 64; display++ ) {
        QString x1 = QString( "/tmp/.X%1-lock" ).arg( display );
        QString x2 = QString( "/tmp/.X11-unix/X%1" ).arg( display );

        if ( QFile::exists( x1 ) or QFile::exists( x2 ) )
            continue;

        else
            break;
    }

    /* Arg3: Get the vt from config.toml */
    QSettings toml( "/etc/greetd/config.toml", QSettings::IniFormat );
    int vt = toml.value( "terminal/vt" ).toInt();

    /* Arg4: Random strings for server auth file */
    QString hash = QCryptographicHash::hash( QDateTime::currentDateTime().toString().toUtf8(), QCryptographicHash::Md5 ).toHex().left( 10 );

    return xinit.arg( base ).arg( display ).arg( vt ).arg( hash );
};

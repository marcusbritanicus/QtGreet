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

// Local Headers
#include "Global.hpp"
#include "QtGreet.hpp"
#include "LayoutManager.hpp"

QSize mScreenSize;
QSettings *sett;
QSettings *users;

static const char *COLOR_INFO		= "\033[01;32m";
static const char *COLOR_DEBUG		= "\033[01;30m";
static const char *COLOR_WARN		= "\033[01;33m";
static const char *COLOR_CRITICAL	= "\033[01;31m";
static const char *COLOR_FATAL		= "\033[01;41m";
static const char *COLOR_RESET		= "\033[00;00m";

void Logger( QtMsgType type, const QMessageLogContext &context, const QString &msg ) {

	if ( msg.startsWith( "Using QCharRef with an index pointing" ) )
		return;

    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";

    switch (type) {
        case QtInfoMsg: {
            fprintf(  stderr, "%s[I]: (%s:%u, %s) %s%s\n",
                COLOR_INFO,
                file,
                context.line,
                function,
                localMsg.constData(),
                COLOR_RESET
            );
            fflush( stderr );

            break;
        }

        case QtDebugMsg: {
            fprintf( stderr, "%s[D]: (%s:%u, %s) %s%s\n",
                COLOR_DEBUG,
                file,
                context.line,
                function,
                localMsg.constData(),
                COLOR_RESET
            );
            fflush( stderr );

            break;
        }

        case QtWarningMsg: {
            fprintf( stderr, "%s[W]: (%s:%u, %s) %s%s\n",
                COLOR_WARN,
                file,
                context.line,
                function,
                localMsg.constData(),
                COLOR_RESET
            );
            fflush( stderr );

            break;
        }

        case QtCriticalMsg: {
            fprintf( stderr, "%s[E]: (%s:%u, %s) %s%s\n",
                COLOR_CRITICAL,
                file,
                context.line,
                function,
                localMsg.constData(),
                COLOR_RESET
            );
            fflush( stderr );

            break;
        }

        case QtFatalMsg: {
            fprintf( stderr, "%s[#]: (%s:%u, %s) %s%s\n",
                COLOR_FATAL,
                file,
                context.line,
                function,
                localMsg.constData(),
                COLOR_RESET
            );
            fflush( stderr );

            break;
        }
    }
};

int main( int argc, char **argv ) {

	QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
	qInstallMessageHandler( Logger );

	QApplication app( argc, argv );

	/** Screen Size */
	mScreenSize = app.primaryScreen()->size();

	/** Settings Objects */
	sett = new QSettings( "/etc/qtgreet/config.ini", QSettings::IniFormat );
	users = new QSettings( "/etc/qtgreet/users.conf", QSettings::IniFormat );

	QtGreet::UI *qtgreet = new QtGreet::UI();
	qtgreet->showFullScreen();

	// QtGreet::Layout *lyt = new QtGreet::Layout();
    // lyt->generateLayout( argv[ 1 ] );

	return app.exec();
};

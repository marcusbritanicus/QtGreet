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

// Local Headers
#include "Global.hpp"
#include "QtGreet.hpp"
#include "LayoutManager.hpp"

#include <unistd.h>

#include <wayqt/WlGlobal.hpp>
#include <wayqt/Application.hpp>
#include <wayqt/Registry.hpp>
#include <wayqt/WindowManager.hpp>

QSize     mScreenSize;
QSettings *sett;
QSettings *users;

static const char *COLOR_INFO     = "\033[01;32m";
static const char *COLOR_DEBUG    = "\033[01;30m";
static const char *COLOR_WARN     = "\033[01;33m";
static const char *COLOR_CRITICAL = "\033[01;31m";
static const char *COLOR_FATAL    = "\033[01;41m";
static const char *COLOR_RESET    = "\033[00;00m";

void Logger( QtMsgType type, const QMessageLogContext& context, const QString& msg ) {
    if ( msg.startsWith( "Using QCharRef with an index pointing" ) ) {
        return;
    }

    QByteArray localMsg  = msg.toLocal8Bit();
    const char *file     = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";

    switch ( type ) {
        case QtInfoMsg: {
            fprintf( stderr, "%s[I]: (%s:%u, %s) %s%s\n",
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
}


void setupWindow( QScreen *screen, WQt::WindowHandle *hndl ) {
    wl_output *output = WQt::getWlOutput( screen );
    hndl->setFullScreen( output );

    while ( hndl->appId().isEmpty() ) {
        qApp->processEvents();
        usleep( 10 * 1000 );
    }
}


int main( int argc, char **argv ) {
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
    qInstallMessageHandler( Logger );

    WQt::Application *app = new WQt::Application( "QtGreet", argc, argv );

    /** Settings Objects */
    sett  = new QSettings( "/etc/qtgreet/config.ini", QSettings::IniFormat );
    users = new QSettings( "/etc/qtgreet/users.conf", QSettings::IniFormat );

    QList<QScreen *> screens;

    for ( QScreen *scrn: app->screens() ) {
        screens << scrn;
    }

    WQt::WindowManager *wm = app->waylandRegistry()->windowManager();
    QObject::connect(
        wm, &WQt::WindowManager::newTopLevelHandle, [=] ( WQt::WindowHandle *hndl ) mutable {
            /** Wait for app ID to be set */
            while ( hndl->appId().isEmpty() ) {
                qApp->processEvents();
                usleep( 10 * 1000 );
            }

            /** If we have used up all the screens, then this is probably a dialog */
            if ( not screens.count() ) {
                return;
            }

            /** Ensure that this handle is indeed qtrgeet */
            if ( not hndl->appId().contains( "qtgreet" ) ) {
                return;
            }

            QScreen *scrn = screens.takeAt( 0 );
            setupWindow( scrn, hndl );
        }
    );

    /** Open a window for every existing screen */
    for ( int i = 0; i < screens.count(); i++ ) {
        QtGreet::UI *ui = new QtGreet::UI();
        ui->showFullScreen();
    }

    /** Now we can wait for new screens to be added */
    QObject::connect(
        app, &QApplication::screenAdded, [ = ] ( QScreen *scrn ) mutable {
            screens << scrn;

            QtGreet::UI *ui = new QtGreet::UI();
            ui->show();
        }
    );

    return app->exec();
}

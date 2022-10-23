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

#include <DFL/DF5/Application.hpp>

#include <wayqt/WayQtUtils.hpp>
#include <wayqt/Registry.hpp>
#include <wayqt/WindowManager.hpp>

QSettings *sett;
QSettings *users;

/** To store various paths */
QString configPath = "/etc/qtgreet/config.ini";
QString usersPath  = "/etc/qtgreetd/users.conf";
QString wlSessPath = "default";
QString xSessPath  = "default";
QString passwdPath = "/etc/passwd";
QString loginPath  = "/etc/login.defs";
QString xrcPath    = "/etc/X11/xinit/xserverrc";
QString tmpPath    = "/tmp/";
QString greetdPath = "/etc/greetd/config.toml";

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
    wl_output *output = WQt::Utils::wlOutputFromQScreen( screen );

    hndl->setFullScreen( output );

    while ( hndl->appId().isEmpty() ) {
        qApp->processEvents();
        usleep( 10 * 1000 );
    }
}


int main( int argc, char **argv ) {
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
    qInstallMessageHandler( Logger );

    DFL::Application *app = new DFL::Application( argc, argv );
    app->setOrganizationName( "QtGreet" );
    app->setApplicationName( "QtGreet" );
    app->setDesktopFileName( "qtgreet.desktop" );
    app->setApplicationVersion( PROJECT_VERSION );

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    /* Optional: Provide config path */
    parser.addOption( { { "c", "config" }, "Configuration file", "cfgfile" } );

    /* Optional: Provide users-data path */
    parser.addOption( { { "d", "data" }, "File to store user data", "datafile" } );

    /* Optional: Provide users-data path */
    parser.addOption( { { "w", "wl-session-path" }, "Path containing wayland session desktops", "wlpath" } );

    /* Optional: Provide users-data path */
    parser.addOption( { { "x", "x-session-path" }, "Path containing x11 session desktops", "xpath" } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "p", "passwd-path" }, "Path to passwd", "passwd" } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "l", "login-defs-path" }, "Path to login.defs", "login" } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "r", "xserver-path" }, "Path to xserverrc", "xrc-path" } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "t", "tmp-path" }, "System temp folder", "tmp" } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "g", "greetd-path" }, "Greetd configuration file", "greetd-path" } );

    /* Optional: Provide login.defs path */
    QCommandLineOption test( "test", "Test QtGreet without using Wayland Protocols" );
    test.setFlags( QCommandLineOption::HiddenFromHelp );
    parser.addOption( test );

    /* Process the CLI args */
    parser.process( *app );

    /** Set the paths */
    configPath = (parser.isSet( "config" ) ? parser.value( "config" ) : configPath);
    usersPath  = (parser.isSet( "data" ) ? parser.value( "data" ) : usersPath);
    wlSessPath = (parser.isSet( "wl-session-path" ) ? parser.value( "wl-session-path" ) : wlSessPath);
    xSessPath  = (parser.isSet( "x-session-path" ) ? parser.value( "x-session-path" ) : xSessPath);
    passwdPath = (parser.isSet( "passwd-path" ) ? parser.value( "passwd-path" ) : passwdPath);
    loginPath  = (parser.isSet( "login-defs-path" ) ? parser.value( "login-defs-path" ) : loginPath);
    xrcPath    = (parser.isSet( "xserver-path" ) ? parser.value( "xserver-path" ) : xrcPath);
    tmpPath    = (parser.isSet( "tmp-path" ) ? parser.value( "tmp-path" ) : tmpPath);
    greetdPath = (parser.isSet( "greetd-path" ) ? parser.value( "greetd-path" ) : greetdPath);

    /** Settings Objects */
    sett  = new QSettings( configPath, QSettings::IniFormat );
    users = new QSettings( usersPath, QSettings::IniFormat );

    QIcon::setThemeName( sett->value( "IconTheme", "hicolor" ).toString() );

    QList<QScreen *> screens;

    for ( QScreen *scrn: app->screens() ) {
        screens << scrn;
    }

    WQt::Registry *wlRegistry = new WQt::Registry( WQt::Wayland::display() );
    wlRegistry->setup();

    WQt::WindowManager *wm = wlRegistry->windowManager();
    QObject::connect(
        wm, &WQt::WindowManager::newTopLevelHandle, [ = ] ( WQt::WindowHandle *hndl ) mutable {
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

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
#include <signal.h>

#include <DFL/DF5/Application.hpp>
#include <DFL/DF5/Utils.hpp>

#include <wayqt/WayQtUtils.hpp>
#include <wayqt/Registry.hpp>
#include <wayqt/WindowManager.hpp>

QSettings *sett;
QSettings *users;

/** To store various paths */
QString configPath  = "/etc/qtgreet/config.ini";
QString dynDataPath = DYNPATH;
QString wlSessPath  = "default";
QString xSessPath   = "default";
QString passwdPath  = "/etc/passwd";
QString loginPath   = "/etc/login.defs";
QString xrcPath     = "/etc/X11/xinit/xserverrc";
QString tmpPath     = "/tmp/";
QString vtNum       = "1";
QString logPath     = LOGPATH;

void setupWindow( QScreen *screen, WQt::WindowHandle *hndl ) {
    wl_output *output = WQt::Utils::wlOutputFromQScreen( screen );

    hndl->setFullScreen( output );
}


int main( int argc, char **argv ) {
    DFL::Application *app = new DFL::Application( argc, argv );
    app->setOrganizationName( "QtGreet" );
    app->setApplicationName( "QtGreet" );
    app->setDesktopFileName( "qtgreet.desktop" );
    app->setApplicationVersion( PROJECT_VERSION );

    app->interceptSignal( SIGSEGV, true );
    app->interceptSignal( SIGINT, true );
    app->interceptSignal( SIGQUIT, true );
    app->interceptSignal( SIGTERM, true );
    app->interceptSignal( SIGABRT, true );

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    /* Optional: Provide config path */
    parser.addOption( { { "c", "config" }, "Configuration file", "cfgfile" } );

    /* Optional: Provide dynamic data storage path */
    parser.addOption( { { "d", "data-path" }, "Path to store dynamic data", "datafile" } );

    /* Optional: Provide users-data path */
    parser.addOption( { { "w", "wl-session-path" }, "Path(s) containing wayland session desktops", "path1;path2;..." } );

    /* Optional: Provide users-data path */
    parser.addOption( { { "x", "x-session-path" }, "Path(s) containing x11 session desktops", "path1;path2;..." } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "p", "passwd-path" }, "Path to passwd", "passwd" } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "l", "login-defs-path" }, "Path to login.defs", "login" } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "r", "xserver-path" }, "Path to xserverrc", "xrc-path" } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "t", "tmp-path" }, "System temp folder", "tmp" } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "n", "vt-number" }, "VT number where the session is started", "vtnr" } );

    /* Optional: Provide login.defs path */
    parser.addOption( { { "o", "log-path" }, "Path to store log files", "log-path" } );

    /* Optional: Provide login.defs path */
    QCommandLineOption test( "test", "Test QtGreet without using Wayland Protocols" );
    test.setFlags( QCommandLineOption::HiddenFromHelp );
    parser.addOption( test );

    /* Process the CLI args */
    parser.process( *app );

    /** Set the various paths */
    configPath  = (parser.isSet( "config" ) ? parser.value( "config" ) : configPath);
    dynDataPath = (parser.isSet( "data-path" ) ? parser.value( "data-path" ) : dynDataPath);
    wlSessPath  = (parser.isSet( "wl-session-path" ) ? parser.value( "wl-session-path" ) : wlSessPath);
    xSessPath   = (parser.isSet( "x-session-path" ) ? parser.value( "x-session-path" ) : xSessPath);
    passwdPath  = (parser.isSet( "passwd-path" ) ? parser.value( "passwd-path" ) : passwdPath);
    loginPath   = (parser.isSet( "login-defs-path" ) ? parser.value( "login-defs-path" ) : loginPath);
    xrcPath     = (parser.isSet( "xserver-path" ) ? parser.value( "xserver-path" ) : xrcPath);
    tmpPath     = (parser.isSet( "tmp-path" ) ? parser.value( "tmp-path" ) : tmpPath);
    vtNum       = (parser.isSet( "vt-number" ) ? parser.value( "vt-number" ) : vtNum);
    logPath     = (parser.isSet( "log-path" ) ? parser.value( "log-path" ) : logPath);

    /**
     * Notes: Log path
     * 1. logPath / greeter (mode 0700) will contain log files produced by qtgreet's debug output
     * 2. logPath / session (mode 01777) will contain log files produced by the session.
     *    This is redirected from the stdout/stderr in GreetdLogin when starting a named session.
     */

    /** Create @logPath first. */
    if ( not QFile::exists( logPath ) ) {
        mkdir( logPath.toLocal8Bit().constData(), 01777 );
    }

    /** Prepare our paths */
    if ( QFile::exists( logPath ) ) {
        /** Create the 'logPath / greeter' folder */
        QString greeter = QDir( logPath ).filePath( "greeter" );
        QString session = QDir( logPath ).filePath( "session" );

        /** Only qtgreet can write into this folder */
        if ( mkdir( greeter.toLocal8Bit().constData(), 01700 ) ) {
            qDebug() << strerror( errno );
        }

        /** Any user can write into this folder */
        if ( mkdir( session.toLocal8Bit().constData(), 01777 ) ) {
            qDebug() << strerror( errno );
        }
    }

    QString dt( QDateTime::currentDateTime().toString( "yyyyMMddThhmmss" ) );
    QString logFile = QString( "%1/greeter/%2.log" ).arg( logPath ).arg( dt );
    if ( DFL::log == NULL ) {
        /** Unable to open the logPath, we'll try system tmpPath */
        QString altLogFile = QString( "%1/%2.log" ).arg( tmpPath ).arg( dt );
        DFL::log = fopen( altLogFile.toLocal8Bit().data(), "w" );
    }

    DFL::log = fopen( logFile.toLocal8Bit().data(), "w" );
    qInstallMessageHandler( DFL::Logger );

    qInfo() << "Starting QtGreet";

    if ( parser.isSet( "test" ) ) {
        qInfo() << "Config file:           " << configPath;
        qInfo() << "Dynamic data path:     " << dynDataPath;
        qInfo() << "Wayland sessions path: " << wlSessPath;
        qInfo() << "X11 sessions path:     " << xSessPath;
        qInfo() << "passwd file:           " << passwdPath;
        qInfo() << "login.defs file:       " << loginPath;
        qInfo() << "Xserverrc file:        " << xrcPath;
        qInfo() << "System temp path:      " << tmpPath;
        qInfo() << "VT Number:             " << vtNum;
        qInfo() << "Session log path:      " << logPath;
    }

    if ( dynDataPath.isEmpty() and not parser.isSet( "data-path" ) ) {
        qCritical() << "Please use --data-path option to specify where QtGreet can store its dynamic data.";
        parser.showHelp( EXIT_FAILURE );       // Exit with an error
    }

    /** Settings Objects */
    sett  = new QSettings( configPath, QSettings::IniFormat );
    users = new QSettings( dynDataPath + "/users.conf", QSettings::IniFormat );

    /** Set the icon theme name */
    QIcon::setThemeName( sett->value( "IconTheme", "hicolor" ).toString() );

    QList<QScreen *> screens;

    for ( QScreen *scrn: app->screens() ) {
        screens << scrn;
    }

    WQt::Registry *wlRegistry = new WQt::Registry( WQt::Wayland::display() );
    wlRegistry->setup();

    WQt::WindowManager *wm = wlRegistry->windowManager();

    if ( wm ) {
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
    }

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

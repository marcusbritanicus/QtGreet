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

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <QtCore>

extern "C" {
#include "tinywl.h"
}

TinyWlServer *server = NULL;

static const char *COLOR_WARN     = "\033[01;33m";
static const char *COLOR_CRITICAL = "\033[01;31m";
static const char *COLOR_FATAL    = "\033[01;41m";
static const char *COLOR_RESET    = "\033[00;00m";

void handleSignals( int signum ) {
    switch ( signum ) {
        case SIGINT: {
            printf( "%sInterrupt signalled.%s\n", COLOR_WARN, COLOR_RESET );
            break;
        }

        case SIGTERM: {
            printf( "%sPolite term request received.%s\n", COLOR_WARN, COLOR_RESET );
            break;
        }

        case SIGQUIT: {
            printf( "%sSigQuit received.%s\n", COLOR_WARN, COLOR_RESET );
            break;
        }

        default: {
            return;
        }
    }

    printf( "%sClosing down the server.%s\n", COLOR_CRITICAL, COLOR_RESET );
    haltWlEventLoop();
}


static void startQtGreet( int argc, char *argv[] ) {
    QCoreApplication app( argc, argv );

    QProcess *proc = new QProcess();

    QObject::connect(
        proc, &QProcess::readyReadStandardError, [ = ] () {
            /** Redirect the stderr of qtgreet to our stderr */
            qDebug() << proc->readAllStandardError().trimmed().data();
        }
    );

    QObject::connect(
        proc, &QProcess::readyReadStandardOutput, [ = ] () {
            /** Redirect the stdout of qtgreet to our stderr */
            qDebug() << proc->readAllStandardOutput().trimmed().data();
        }
    );

    /**
     * Pass the arguments sent to greetwl to qtgreet.
     * This way we can quite easily use the cli args of qtgreet while using greetwl
     */
    proc->start( "qtgreet", app.arguments().mid( 1 ) );
    proc->waitForFinished( -1 );
}


int main( int argc, char **argv ) {
    /** Init TinyWlServer */
    server = new TinyWlServer();

    /** Start the compositor */
    if ( !startCompositor() ) {
        printf( "%sFailed to start TinyWL Compositor%s\n", COLOR_FATAL, COLOR_RESET );
        return -1;
    }

    /** Setup the environment variables for Qt to start properly */
    setupEnv();

    if ( fork() == 0 ) {
        startQtGreet( argc, argv );

        /** Request the parent to shutdown */
        kill( getppid(), SIGTERM );
    }

    signal( SIGINT,  handleSignals );
    signal( SIGTERM, handleSignals );
    signal( SIGQUIT, handleSignals );

    /** Run the wayland event loop instead of QApplication. Will this work? */
    runWlEventLoop();

    closeCompositor();

    delete server;

    return 0;
}

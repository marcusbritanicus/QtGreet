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

QString getCommand( QString what ) {
    QString cmd = sett->value( what );

    /** Figure out if we should use systemctl or loginctl */
    if ( cmd == "default" ) {
        for ( QString path: qEnvironmentVariable( "PATH" ).split( ":" ) ) {
            if ( QFile::exists( path + "/systemctl" ) ) {
                return "systemctl";
            }

            if ( QFile::exists( path + "/loginctl" ) ) {
                return "loginctl";
            }
        }

        qCritical() << "Unable to figure out command for" << what;
        return QString();
    }

    return cmd;
}


PowerButton::PowerButton( QString type ) : QToolButton() {
    setAutoRaise( true );
    setFixedSize( QSize( 40, 40 ) );
    setIconSize( QSize( 32, 32 ) );

    if ( type == "Menu" ) {
        setIcon( QIcon( ":/icons/shutdown.png" ) );

        setToolTip( "Logout Menu" );

        QMenu *menu = new QMenu();
        menu->addAction(
            QIcon( ":/icons/suspend.png" ), "Suspend to RAM", [ = ] () {
                QProcess::startDetached( getCommand( "suspend" ), { "suspend", "-i" } );
            }
        );
        menu->addAction(
            QIcon( ":/icons/hibernate.png" ), "Suspend to Disk", [ = ] () {
                QProcess::startDetached( getCommand( "hibernate" ), { "hibernate", "-i" } );
            }
        );
        menu->addAction(
            QIcon( ":/icons/shutdown.png" ), "Halt System", [ = ] () {
                QProcess::startDetached( getCommand( "shutdown" ), { "shutdown", "-i" } );
            }
        );
        menu->addAction(
            QIcon( ":/icons/reboot.png" ), "Reboot System", [ = ] () {
                QProcess::startDetached( getCommand( "reboot" ), { "reboot", "-i" } );
            }
        );

        setMenu( menu );
        setPopupMode( QToolButton::InstantPopup );
    }

    else if ( type == "Suspend" ) {
        setObjectName( "Suspend" );
        setToolTip( "Suspend to RAM" );
        setIcon( QIcon( ":/icons/suspend.png" ) );
        connect(
            this, &QToolButton::clicked, [ = ] () {
                QProcess::startDetached( getCommand( "suspend" ), { "suspend", "-i" } );
            }
        );
    }

    else if ( type == "Hibernate" ) {
        setObjectName( "Hibernate" );
        setToolTip( "Suspend to Disk" );
        setIcon( QIcon( ":/icons/hibernate.png" ) );
        connect(
            this, &QToolButton::clicked, [ = ] () {
                QProcess::startDetached( getCommand( "hibernate" ), { "hibernate", "-i" } );
            }
        );
    }

    else if ( type == "Halt" ) {
        setObjectName( "Halt" );
        setToolTip( "Shutdown the system" );
        setIcon( QIcon( ":/icons/shutdown.png" ) );
        connect(
            this, &QToolButton::clicked, [ = ] () {
                QProcess::startDetached( getCommand( "shutdown" ), { "shutdown", "-i" } );
            }
        );
    }

    else if ( type == "Reboot" ) {
        setObjectName( "Reboot" );
        setToolTip( "Reboot the system" );
        setIcon( QIcon( ":/icons/reboot.png" ) );
        connect(
            this, &QToolButton::clicked, [ = ] () {
                QProcess::startDetached( getCommand( "reboot" ), { "reboot", "-i" } );
            }
        );
    }
}

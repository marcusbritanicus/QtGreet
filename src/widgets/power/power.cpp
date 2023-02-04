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

#include "Global.hpp"
#include "power.hpp"

QString getCommand( QString what ) {
    return sett->value( "PowerCommands/" + what ).toString();
}


PowerButton::PowerButton( QString type ) : QToolButton() {
    setAutoRaise( true );
    setFixedSize( QSize( 40, 40 ) );
    setIconSize( QSize( 32, 32 ) );

    if ( type == "Menu" ) {
        setIcon( QIcon( ":/icons/shutdown.png" ) );
        setObjectName( "PowerButton" );

        setToolTip( "Logout Menu" );

        QMenu *menu = new QMenu();
        menu->addAction(
            QIcon( ":/icons/suspend.png" ), "Suspend to RAM", [ = ] () {
                QStringList args = getCommand( "Suspend" ).split( " " );
                if ( args.at( 0 ) == "dbus" ) {
                    emit suspend();
                }

                else {
                    QProcess::startDetached( args.takeAt( 0 ), args );
                }
            }
        );

        menu->addAction(
            QIcon( ":/icons/hibernate.png" ), "Suspend to Disk", [ = ] () {
                QStringList args = getCommand( "Hibernate" ).split( " " );
                if ( args.at( 0 ) == "dbus" ) {
                    emit hibernate();
                }

                else {
                    QProcess::startDetached( args.takeAt( 0 ), args );
                }
            }
        );

        menu->addAction(
            QIcon( ":/icons/shutdown.png" ), "Halt System", [ = ] () {
                QStringList args = getCommand( "Shutdown" ).split( " " );
                if ( args.at( 0 ) == "dbus" ) {
                    emit shutdown( false );
                }

                else {
                    QProcess::startDetached( args.takeAt( 0 ), args );
                    emit shutdown( true );
                }
            }
        );

        menu->addAction(
            QIcon( ":/icons/reboot.png" ), "Reboot System", [ = ] () {
                QStringList args = getCommand( "Reboot" ).split( " " );
                if ( args.at( 0 ) == "dbus" ) {
                    emit reboot( false );
                }

                else {
                    QProcess::startDetached( args.takeAt( 0 ), args );
                    emit reboot( true );
                }
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
                QStringList args = getCommand( "Suspend" ).split( " " );
                if ( args.at( 0 ) == "dbus" ) {
                    emit suspend();
                }

                else {
                    QProcess::startDetached( args.takeAt( 0 ), args );
                }
            }
        );
    }

    else if ( type == "Hibernate" ) {
        setObjectName( "Hibernate" );
        setToolTip( "Suspend to Disk" );
        setIcon( QIcon( ":/icons/hibernate.png" ) );
        connect(
            this, &QToolButton::clicked, [ = ] () {
                QStringList args = getCommand( "Hibernate" ).split( " " );
                if ( args.at( 0 ) == "dbus" ) {
                    emit hibernate();
                }

                else {
                    QProcess::startDetached( args.takeAt( 0 ), args );
                }
            }
        );
    }

    else if ( type == "Halt" ) {
        setObjectName( "Halt" );
        setToolTip( "Shutdown the system" );
        setIcon( QIcon( ":/icons/shutdown.png" ) );
        connect(
            this, &QToolButton::clicked, [ = ] () {
                QStringList args = getCommand( "Shutdown" ).split( " " );
                if ( args.at( 0 ) == "dbus" ) {
                    emit shutdown( false );
                }

                else {
                    QProcess::startDetached( args.takeAt( 0 ), args );
                    emit shutdown( true );
                }
            }
        );
    }

    else if ( type == "Reboot" ) {
        setObjectName( "Reboot" );
        setToolTip( "Reboot the system" );
        setIcon( QIcon( ":/icons/reboot.png" ) );
        connect(
            this, &QToolButton::clicked, [ = ] () {
                QStringList args = getCommand( "Reboot" ).split( " " );
                if ( args.at( 0 ) == "dbus" ) {
                    emit reboot( false );
                }

                else {
                    QProcess::startDetached( args.takeAt( 0 ), args );
                    emit reboot( true );
                }
            }
        );
    }
}

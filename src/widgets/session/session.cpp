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
#include "Global.hpp"

#include "session.hpp"

static bool IsExec( QString exec ) {
    /* If this is a full path (ex: /usr/bin/fish) */
    if ( access( exec.toUtf8().constData(), R_OK | X_OK ) == 0 ) {
        return true;
    }

    QStringList parts = exec.split( QRegularExpression( "\\s+" ) );
    QString     cmd;

    /** Pick the first part which is not a env variable as cmd */
    for ( QString part: parts ) {
        if ( part.contains( "=" ) ) {
            continue;
        }

        cmd = part;
        break;
    }

    /* Otherwise (ex: plasmashell) */
    for ( QString path : qgetenv( "PATH" ).split( ':' ) ) {
        if ( QFileInfo( path + "/" + exec ).isExecutable() ) {
            return true;
        }

        if ( QFileInfo( path + "/" + cmd ).isExecutable() ) {
            return true;
        }
    }

    return false;
}


Sessions getSessions( bool custom ) {
    Sessions mSessions;

    QStringList wlSessPaths;

    if ( wlSessPath == "default" ) {
        wlSessPaths = QStandardPaths::locateAll( QStandardPaths::GenericDataLocation, "wayland-sessions", QStandardPaths::LocateDirectory );
    }

    else {
        wlSessPaths = wlSessPath.split( ";", Qt::SkipEmptyParts );
    }

    for ( QString wlSessDir: wlSessPaths ) {
        for ( QFileInfo sess: QDir( wlSessDir ).entryInfoList( { "*.desktop" } ) ) {
            QSettings session( sess.absoluteFilePath(), QSettings::IniFormat );

            bool isAvailable = IsExec( session.value( "Desktop Entry/TryExec" ).toString() );
            isAvailable |= IsExec( session.value( "Desktop Entry/Exec" ).toString() );

            if ( isAvailable ) {
                Session s = Session{
                    session.value( "Desktop Entry/Name" ).toString(),
                    session.value( "Desktop Entry/Icon", ":/icons/session.png" ).toString(),
                    "wayland",
                    session.value( "Desktop Entry/Exec" ).toString(),
                    sess.absoluteFilePath()
                };

                if ( not s.name.contains( "wayland", Qt::CaseInsensitive ) ) {
                    s.name += " (Wayland)";
                }

                mSessions << s;
            }
        }
    }

    QStringList xSessPaths;

    if ( xSessPath == "default" ) {
        xSessPaths = QStandardPaths::locateAll( QStandardPaths::GenericDataLocation, "xsessions", QStandardPaths::LocateDirectory );
    }

    else {
        xSessPaths = xSessPath.split( ";", Qt::SkipEmptyParts );
    }

    /** Support for launching session defined in ~/.xinitrc. */
    if ( IsExec( "startx" ) ) {
        Session defaultX11 = Session {
            "Default X11 Session",
            ":/icons/session.png",
            "X11",
            "startx",
            "/usr/share/xsessions/startx.desktop"
        };

        mSessions << defaultX11;
    }

    for ( QString xSessDir: xSessPaths ) {
        for ( QFileInfo sess: QDir( xSessDir ).entryInfoList( { "*.desktop" } ) ) {
            QSettings session( sess.absoluteFilePath(), QSettings::IniFormat );

            bool isAvailable = IsExec( session.value( "Desktop Entry/TryExec" ).toString() );
            isAvailable |= IsExec( session.value( "Desktop Entry/Exec" ).toString() );

            if ( isAvailable ) {
                Session s = Session{
                    session.value( "Desktop Entry/Name" ).toString() + " (X11)",
                    session.value( "Desktop Entry/Icon", ":/icons/session.png" ).toString(),
                    "X11",
                    session.value( "Desktop Entry/Exec" ).toString(),
                    sess.absoluteFilePath()
                };

                mSessions << s;
            }
        }
    }

    if ( custom ) {
        Session customSess{ "Custom", "application-x-executable", "unknown", "", "" };
        mSessions << customSess;
    }

    return mSessions;
}


SessionEdit::SessionEdit() : QLineEdit() {
    setObjectName( "SessionEdit" );
    setMinimumSize( QSize( 200, 27 ) );
    setAlignment( Qt::AlignCenter );
}


/* Session Name - Generic Class */

SessionName::SessionName( bool custom ) {
    mSessionList = getSessions( custom );
}


void SessionName::switchToNextSession() {
    curSess++;

    if ( curSess >= mSessionList.count() ) {
        curSess = 0;
    }
}


void SessionName::switchToPreviousSession() {
    curSess--;

    if ( curSess < 0 ) {
        curSess = mSessionList.count() - 1;
    }
}


Sessions SessionName::sessions() {
    return mSessionList;
}


Session SessionName::currentSession() {
    // Return an invalid session
    return mSessionList.at( curSess );
}


/* Session ComboBox */

SessionCombo::SessionCombo( bool custom ) : QComboBox(), SessionName( custom ) {
    setObjectName( "SessionCombo" );
    for ( Session sess: mSessionList ) {
        addItem( QIcon( sess.icon ), sess.name );
    }
}


void SessionCombo::switchToNextSession() {
    SessionName::switchToNextSession();
    setCurrentIndex( curSess );
}


void SessionCombo::switchToPreviousSession() {
    SessionName::switchToPreviousSession();
    setCurrentIndex( curSess );
}


/* Session List */

SessionList::SessionList( bool custom ) : QListWidget(), SessionName( custom ) {
    setObjectName( "SessionList" );
    setIconSize( QSize( 24, 24 ) );
    setFont( QFont( "Quicksand", 12 ) );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    for ( Session sess: mSessionList ) {
        addItem( new QListWidgetItem( QIcon::fromTheme( sess.icon, QIcon( sess.icon ) ), sess.name, this ) );
    }
}


void SessionList::switchToNextSession() {
    SessionName::switchToNextSession();
    setCurrentRow( curSess );
}


void SessionList::switchToPreviousSession() {
    SessionName::switchToPreviousSession();
    setCurrentRow( curSess );
}


/* Session Label */

SessionLabel::SessionLabel( bool custom ) : QWidget(), SessionName( custom ) {
    setObjectName( "SessionLabel" );

    prevBtn = new QToolButton();
    prevBtn->setObjectName( "SessionNavLeft" );
    prevBtn->setIcon( QIcon( ":/icons/arrow-left.png" ) );

    lbl = new QLabel();
    lbl->setObjectName( "SessionLabel" );

    nextBtn = new QToolButton();
    nextBtn->setObjectName( "SessionNavRight" );
    nextBtn->setIcon( QIcon( ":/icons/arrow-right.png" ) );

    QHBoxLayout *lyt = new QHBoxLayout();

    lyt->setContentsMargins( QMargins() );
    lyt->setSpacing( 5 );

    lyt->addWidget( prevBtn );
    lyt->addWidget( lbl );
    lyt->addWidget( nextBtn );

    setLayout( lyt );

    nextBtn->hide();
    prevBtn->hide();

    setFixedHeight( 27 );
}


void SessionLabel::setText( QString text ) {
    lbl->setText( text );
}


void SessionLabel::setAlignment( Qt::Alignment a ) {
    lbl->setAlignment( a );
}


void SessionLabel::switchToNextSession() {
    SessionName::switchToNextSession();
    setText( mSessionList.at( curSess ).name );
}


void SessionLabel::switchToPreviousSession() {
    SessionName::switchToPreviousSession();
    setText( mSessionList.at( curSess ).name );
}


void SessionLabel::setShowNavButtons( bool show ) {
    if ( show ) {
        prevBtn->show();
        nextBtn->show();
    }

    else {
        prevBtn->hide();
        nextBtn->hide();
    }
}


void SessionLabel::setIconSize( QSize size ) {
    nextBtn->setIconSize( size );
    prevBtn->setIconSize( size );
}


void SessionLabel::setFixedHeight( int h ) {
    QWidget::setFixedHeight( h );

    nextBtn->setFixedWidth( h );
    nextBtn->setFixedHeight( h );

    prevBtn->setFixedWidth( h );
    prevBtn->setFixedHeight( h );
}

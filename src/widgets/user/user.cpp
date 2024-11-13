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

#include <sys/types.h>
#include <pwd.h>

#include "Global.hpp"
#include "user.hpp"

static inline QString getIcon( QString name ) {
    QString iconPath = QString( "/home/%1/.face" ).arg( name );
    
    QPixmap pix( iconPath );

    if ( pix.isNull() ) {
        iconPath = QString( "/home/%1/.face.icon" ).arg( name );
        pix      = QPixmap( iconPath );
    }

    if ( pix.isNull() ) {
        iconPath = QString( "/var/lib/AccountsService/icons/%1" ).arg( name );
        pix      = QPixmap( iconPath );
    }

    if ( pix.isNull() ) {
        iconPath = QString( ":/icons/user.png" );
    }

    return iconPath;
}


static inline Users getUsers() {
    Users mUserList;

    /* Read /etc/login.defs */
    QFile loginDefs( loginPath );

    loginDefs.open( QFile::ReadOnly );
    QString defs = QString::fromLocal8Bit( loginDefs.readAll() );

    loginDefs.close();

    /* Default values */
    uid_t uid_min = 1000;
    uid_t uid_max = 60000;

    for ( QString def: defs.split( "\n", Qt::SkipEmptyParts ) ) {
        if ( def.startsWith( "UID_MIN" ) ) {
            uid_min = def.simplified().split( " ", Qt::SkipEmptyParts ).value( 1 ).toInt();
        }

        if ( def.startsWith( "UID_MAX" ) ) {
            uid_max = def.simplified().split( " ", Qt::SkipEmptyParts ).value( 1 ).toInt();
        }
    }

    mUserList.clear();

    while ( true ) {
        struct passwd *pwd = getpwent();

        if ( !pwd ) {
            break;
        }

        /** We want values within our UID limits */
        if ( (pwd->pw_uid < uid_min) or (pwd->pw_uid > uid_max) ) {
            continue;
        }

        User usr = User{
            pwd->pw_name,
            pwd->pw_uid,
            pwd->pw_gid,
            QString( pwd->pw_gecos ).replace( ",", "" ),
            pwd->pw_dir,
            pwd->pw_shell,
            getIcon( pwd->pw_name )
        };

        mUserList << usr;
    }
    endpwent();

    std::sort(
        mUserList.begin(), mUserList.end(), [ = ]( const User& u1, const User& u2 ) {
            return (u1.uid < u2.uid);
        }
    );

    return mUserList;
}


/* User Name - Generic Class */

UserName::UserName() {
    mUserList = getUsers();
    switchToNextUser();
}


void UserName::switchToNextUser() {
    curUser++;

    if ( curUser >= mUserList.count() ) {
        curUser = 0;
    }
}


void UserName::switchToPreviousUser() {
    curUser--;

    if ( curUser < 0 ) {
        curUser = mUserList.count() - 1;
    }
}


Users UserName::users() {
    return mUserList;
}


User UserName::currentUser() {
    if ( curUser == -1 ) {
        return User();
    }

    return mUserList.at( curUser );
}


bool UserName::setCurrentUser( User usr ) {
    /** Check name and uid. */
    for ( int i = 0; i < mUserList.count(); i++ ) {
        User xusr = mUserList.at( i );

        if ( (xusr.username == usr.username) and (xusr.uid == usr.uid) ) {
            curUser = i;
            return true;
        }
    }

    return false;
}


bool UserName::setCurrentUser( uint uid ) {
    /** Check uid. */
    for ( int i = 0; i < mUserList.count(); i++ ) {
        User xusr = mUserList.at( i );

        if ( xusr.uid == uid ) {
            curUser = i;
            return true;
        }
    }

    return false;
}


/* User LineEdit */
UserEdit::UserEdit() : QLineEdit(), UserName() {
    setObjectName( "UserEdit" );
    setPlaceholderText( "Username" );

    QStringList userList;
    for ( User usr: mUserList ) {
        userList << usr.username;
    }

    QCompleter *completer = new QCompleter( userList, this );

    completer->setCaseSensitivity( Qt::CaseInsensitive );
    completer->setCompletionMode( QCompleter::InlineCompletion );
    setCompleter( completer );

    connect(
        this, &QLineEdit::textEdited, [ = ] ( QString text ) {
            for ( User user: mUserList ) {
                if ( user.username == text ) {
                    setCurrentUser( user );
                    return;
                }
            }

            /** Otherwise, we will reset the current user to -1 */
            curUser = -1;
        }
    );
}


void UserEdit::switchToNextUser() {
    UserName::switchToNextUser();

    setText( mUserList[ curUser ].username );
}


void UserEdit::switchToPreviousUser() {
    UserName::switchToPreviousUser();

    setText( mUserList[ curUser ].username );
}


bool UserEdit::setCurrentUser( User usr ) {
    bool ok = UserName::setCurrentUser( usr );

    if ( ok == true ) {
        setText( mUserList[ curUser ].username );
    }

    return ok;
}


bool UserEdit::setCurrentUser( uint uid ) {
    bool ok = UserName::setCurrentUser( uid );

    if ( ok == true ) {
        setText( mUserList[ curUser ].username );
    }

    return ok;
}


/* User ComboBox */

UserCombo::UserCombo() : QComboBox(), UserName() {
    setObjectName( "UserCombo" );

    for ( User usr: mUserList ) {
        addItem( QIcon( usr.icon ), usr.username );
    }
}


void UserCombo::switchToNextUser() {
    UserName::switchToNextUser();

    setCurrentIndex( curUser );
    setToolTip( mUserList.at( curUser ).username );
}


void UserCombo::switchToPreviousUser() {
    UserName::switchToPreviousUser();

    setCurrentIndex( curUser );
    setToolTip( mUserList.at( curUser ).username );
}


bool UserCombo::setCurrentUser( User usr ) {
    bool ok = UserName::setCurrentUser( usr );

    if ( ok == true ) {
        setCurrentIndex( curUser );
    }

    return ok;
}


bool UserCombo::setCurrentUser( uint uid ) {
    bool ok = UserName::setCurrentUser( uid );

    if ( ok == true ) {
        setCurrentIndex( curUser );
    }

    return ok;
}


/* User List */

UserList::UserList() : QListWidget(), UserName() {
    setObjectName( "UserList" );
    setIconSize( QSize( 24, 24 ) );
    setFont( QFont( "Quicksand", 12 ) );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    for ( User usr: mUserList ) {
        QListWidgetItem *item = new QListWidgetItem( QIcon( usr.icon ), usr.username, this );
        item->setData( Qt::UserRole + 1, QVariant::fromValue( usr ) );
        addItem( item );
    }
}


void UserList::switchToNextUser() {
    UserName::switchToNextUser();

    setCurrentRow( curUser );
    setToolTip( mUserList.at( curUser ).username );
}


void UserList::switchToPreviousUser() {
    UserName::switchToPreviousUser();

    setCurrentRow( curUser );
    setToolTip( mUserList.at( curUser ).username );
}


bool UserList::setCurrentUser( User usr ) {
    bool ok = UserName::setCurrentUser( usr );

    if ( ok == true ) {
        setCurrentRow( curUser );
    }

    return ok;
}


bool UserList::setCurrentUser( uint uid ) {
    bool ok = UserName::setCurrentUser( uid );

    if ( ok == true ) {
        setCurrentRow( curUser );
    }

    return ok;
}


/* User Label */

UserLabel::UserLabel() : QWidget(), UserName() {
    setObjectName( "UserLabel" );

    prevBtn = new QToolButton();
    prevBtn->setObjectName( "UserNavLeft" );
    prevBtn->setIcon( QIcon( ":/icons/arrow-left.png" ) );

    lbl = new QLabel();
    lbl->setObjectName( "UserLabel" );

    nextBtn = new QToolButton();
    nextBtn->setObjectName( "UserNavRight" );
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


void UserLabel::setText( QString text ) {
    lbl->setText( text );
}


void UserLabel::setAlignment( Qt::Alignment a ) {
    lbl->setAlignment( a );
}


void UserLabel::switchToNextUser() {
    UserName::switchToNextUser();

    User usr = mUserList.at( curUser );

    lbl->setText( QString( "%1 (%2)" ).arg( usr.name ).arg( usr.username ) );
    setToolTip( mUserList.at( curUser ).username );
}


void UserLabel::switchToPreviousUser() {
    UserName::switchToPreviousUser();

    User usr = mUserList.at( curUser );

    lbl->setText( QString( "%1 (%2)" ).arg( usr.name ).arg( usr.username ) );
    setToolTip( mUserList.at( curUser ).username );
}


bool UserLabel::setCurrentUser( User usr ) {
    bool ok = UserName::setCurrentUser( usr );

    if ( ok == true ) {
        User usr = mUserList.at( curUser );

        lbl->setText( QString( "%1 (%2)" ).arg( usr.name ).arg( usr.username ) );
    }

    return ok;
}


bool UserLabel::setCurrentUser( uint uid ) {
    bool ok = UserName::setCurrentUser( uid );

    if ( ok == true ) {
        User usr = mUserList.at( curUser );

        lbl->setText( QString( "%1 (%2)" ).arg( usr.name ).arg( usr.username ) );
    }

    return ok;
}


void UserLabel::setShowNavButtons( bool show ) {
    if ( show ) {
        prevBtn->show();
        nextBtn->show();
    }

    else {
        prevBtn->hide();
        nextBtn->hide();
    }
}


void UserLabel::setIconSize( QSize size ) {
    nextBtn->setIconSize( size );
    prevBtn->setIconSize( size );
}


void UserLabel::setFixedHeight( int h ) {
    QWidget::setFixedHeight( h );

    nextBtn->setFixedWidth( h );
    nextBtn->setFixedHeight( h );

    prevBtn->setFixedWidth( h );
    prevBtn->setFixedHeight( h );
}

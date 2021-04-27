/*
	*
	* Copyright 2020 Britanicus <marcusbritanicus@gmail.com>
	*
	* This file is a part of QtGreet project
	* (https://gitlab.com/marcusbritanicus/QtGreet/)
	* Qt5 example of analog clock is the inspiration for this.
	* Some parts of the code are taken from Qt5's example.
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

#include <QtWidgets>

#include "user.hpp"

static inline QPixmap getIcon( QString name ) {

    QPixmap pix( QString( "/home/%1/.face" ).arg( name ) );
    if ( pix.isNull() )
        pix = QPixmap( QString( "/home/%1/.face.icon" ).arg( name ) );

    if ( pix.isNull() )
        pix = QPixmap( ":/icons/user.png" );

    return pix;
}

static inline Users getUsers() {

    Users mUserList;

    /* Read /etc/login.defs */
    QFile loginDefs( "/etc/login.defs" );
    loginDefs.open( QFile::ReadOnly );
    QString defs = QString::fromLocal8Bit( loginDefs.readAll() );
    loginDefs.close();

    /* Default values */
    int uid_min = 1000;
    int uid_max = 60000;

    for( QString def: defs.split( "\n", Qt::SkipEmptyParts ) ) {
        if ( def.startsWith( "UID_MIN" ) )
            uid_min = def.simplified().split( " ", Qt::SkipEmptyParts ).value( 1 ).toInt();

        if ( def.startsWith( "UID_MAX" ) )
            uid_max = def.simplified().split( " ", Qt::SkipEmptyParts ).value( 1 ).toInt();
    }

    /* Read and parse /etc/passwd */
    QFile passwd( "/etc/passwd" );
    passwd.open( QFile::ReadOnly );
    QString usrData = QString::fromLocal8Bit( passwd.readAll() );
    passwd.close();

    mUserList.clear();

    // Read /etc/passwd and list the ones with UID > login.defs[UID_MIN]
    for( QString user: usrData.split( "\n", Qt::SkipEmptyParts ) ) {
        QStringList uInfo = user.split( ":", Qt::KeepEmptyParts );
        if ( ( uInfo.value( 2 ).toInt() >= uid_min ) and ( uInfo.value( 3 ).toInt() <= uid_max ) ) {
            User usr = User{
                uInfo.value( 0 ),
                uInfo.value( 2 ).toUInt(),
                uInfo.value( 3 ).toUInt(),
                uInfo.value( 4 ).replace( ",", "" ),
                uInfo.value( 5 ),
                uInfo.value( 6 ),
                getIcon( uInfo.value( 0 ) )
            };

            mUserList << usr;
        }
    }

    std::sort(
        mUserList.begin(), mUserList.end(), [=]( const User &u1, const User &u2 ) {
            return ( u1.uid < u2.uid );
        }
    );

    return mUserList;
};

UserListCombo::UserListCombo() : QComboBox() {

    setObjectName( "UserList" );
    mUserList = getUsers();

    for( User usr: mUserList ) {
        addItem( QIcon( usr.icon ), usr.username, QVariant::fromValue( usr ) );
    }
};

UserList::UserList() : QListWidget() {

    setObjectName( "UserName" );
    setIconSize( QSize( 24, 24 ) );
    setFont( QFont( "Quicksand", 12 ) );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    mUserList = getUsers();

    for( User usr: mUserList ) {
        QListWidgetItem *item = new QListWidgetItem( QIcon( usr.icon ), usr.username, this );
        item->setData( Qt::UserRole + 1, QVariant::fromValue( usr ) );
        addItem( item );
    }
};

UserNameLabel::UserNameLabel() : QLabel() {

    setObjectName( "UserName" );
    setFixedHeight( 27 );
    setText( "Hello, Marcus" );
};

UserNameButton::UserNameButton() : QPushButton() {

    setObjectName( "UserName" );
    mUserList = getUsers();

    setFixedHeight( 27 );
    setText( "Hello, Marcus" );

    switchToNextUser();
};

void UserNameButton::switchToNextUser() {

    curUser++;
    if ( curUser >= mUserList.count() )
        curUser = 0;

    setText( mUserList.at( curUser ).name );
    setToolTip( mUserList.at( curUser ).username );
};

void UserNameButton::switchToPreviousUser() {

    curUser--;
    if ( curUser < 0 )
        curUser = mUserList.count() - 1;

    setText( mUserList.at( curUser ).name );
    setToolTip( mUserList.at( curUser ).username );
};

QPixmap UserNameButton::userIcon() {

    return mUserList.at( curUser ).icon;
};

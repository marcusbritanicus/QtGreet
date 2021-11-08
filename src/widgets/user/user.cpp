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

static inline QString getIcon( QString name ) {

    QString iconPath = QString( "/home/%1/.face" ).arg( name );

    QPixmap pix( QString( "/home/%1/.face" ).arg( name ) );
    if ( pix.isNull() ) {
        pix = QPixmap( QString( "/home/%1/.face.icon" ).arg( name ) );
        iconPath = QString( "/home/%1/.face.icon" ).arg( name );
    }

    if ( pix.isNull() )
        iconPath = QString( ":/icons/user.png" );

    return iconPath;
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

/* User Name - Generic Class */

UserName::UserName() {

    mUserList = getUsers();
    switchToNextUser();
};

void UserName::switchToNextUser() {

    curUser++;
    if ( curUser >= mUserList.count() )
        curUser = 0;
};

void UserName::switchToPreviousUser() {

    curUser--;
    if ( curUser < 0 )
        curUser = mUserList.count() - 1;
};

Users UserName::users() {

    return mUserList;
};

User UserName::currentUser() {

    return mUserList.at( curUser );
};

void UserName::setCurrentUser( User usr ) {

    /** Check name and uid. */
    for( int i = 0; i < mUserList.count(); i++ ) {
        User xusr = mUserList.at( i );
        if ( ( xusr.username == usr.username ) and ( xusr.uid == usr.uid ) ) {
            curUser = i;
            break;
        }
    }
};

/* User ComboBox */

UserCombo::UserCombo() : QComboBox(), UserName() {

    setObjectName( "UserCombo" );
    mUserList = getUsers();

    for( User usr: mUserList )
        addItem( QIcon( usr.icon ), usr.username );
};

void UserCombo::switchToNextUser() {

    UserName::switchToNextUser();

    setCurrentIndex( curUser );
    setToolTip( mUserList.at( curUser ).username );
};

void UserCombo::switchToPreviousUser() {

    UserName::switchToPreviousUser();

    setCurrentIndex( curUser );
    setToolTip( mUserList.at( curUser ).username );
};

/* User List */

UserList::UserList() : QListWidget(), UserName() {

    setObjectName( "UserList" );
    setIconSize( QSize( 24, 24 ) );
    setFont( QFont( "Quicksand", 12 ) );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    for( User usr: mUserList ) {
        QListWidgetItem *item = new QListWidgetItem( QIcon( usr.icon ), usr.username, this );
        item->setData( Qt::UserRole + 1, QVariant::fromValue( usr ) );
        addItem( item );
    }
};

void UserList::switchToNextUser() {

    UserName::switchToNextUser();

    setCurrentRow( curUser );
    setToolTip( mUserList.at( curUser ).username );
};

void UserList::switchToPreviousUser() {

    UserName::switchToPreviousUser();

    setCurrentRow( curUser );
    setToolTip( mUserList.at( curUser ).username );
};

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
};

void UserLabel::setText( QString text ) {

    lbl->setText( text );
};

void UserLabel::setAlignment( Qt::Alignment a ) {

    lbl->setAlignment( a );
};

void UserLabel::switchToNextUser() {

    UserName::switchToNextUser();

    lbl->setText( mUserList.at( curUser ).name );
    setToolTip( mUserList.at( curUser ).username );
};

void UserLabel::switchToPreviousUser() {

    UserName::switchToPreviousUser();

    lbl->setText( mUserList.at( curUser ).name );
    setToolTip( mUserList.at( curUser ).username );
};

void UserLabel::setShowNavButtons( bool show ) {

    if ( show ) {
        prevBtn->show();
        nextBtn->show();
    }

    else {
        prevBtn->hide();
        nextBtn->hide();
    }
};

void UserLabel::setIconSize( QSize size ) {

    nextBtn->setIconSize( size );
    prevBtn->setIconSize( size );
};

void UserLabel::setFixedHeight( int h ) {

    QWidget::setFixedHeight( h );

    nextBtn->setFixedWidth( h );
    nextBtn->setFixedHeight( h );

    prevBtn->setFixedWidth( h );
    prevBtn->setFixedHeight( h );
};

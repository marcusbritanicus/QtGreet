/*
	*
	* Copyright 2020 Britanicus <marcusbritanicus@gmail.com>
	*
	* This file is a part of QtGreet project (https://gitlab.com/marcusbritanicus/QtGreet)
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

#include "QtGreet.hpp"
#include "LayoutManager.hpp"

#include "buttons.hpp"
#include "labels.hpp"
#include "power.hpp"
#include "session.hpp"
#include "user.hpp"

#include <unistd.h>
#include <QtDBus>

#define CONSOLEKIT_SERVICE      "org.freedesktop.ConsoleKit"
#define CONSOLEKIT_PATH         "/org/freedesktop/ConsoleKit/Manager"
#define CONSOLEKIT_INTERFACE    "org.freedesktop.ConsoleKit.Manager"

#define SYSTEMD_SERVICE         "org.freedesktop.login1"
#define SYSTEMD_PATH            "/org/freedesktop/login1"
#define SYSTEMD_INTERFACE       "org.freedesktop.login1.Manager"

static inline int getFormFactor() {
	/*
		*
		* Form Factors
		* ------------
		*
		* 0: Desktop - Typically 14" to 16" screens
		* 1: Tablets - Typically  7" to 10" screens
		* 2: Mobiles - Typically  4" to  6" screens
		* 3: Monitor - Typically larger than 16" - huge sizes, we're not built for these screens.
		*
	*/

	QSizeF screenSize = QGuiApplication::primaryScreen()->physicalSize();
	double diag = sqrt( pow( screenSize.width(), 2 ) + pow( screenSize.height(), 2 ) );

	//- Deadly large screen mobiles
	if ( diag <= 6.5 )
		return 2;

	//- Tablets and small netbooks
	else if ( diag <= 10.1 )
		return 1;

	//- Typical desktops
	else if ( diag <= 16.0 )
		return 0;

	// Large displays
	else
		return 3;

	/* Default value */
	return 0;
};

static QString getStyleSheet( QString fn ) {

    QFile qss( fn );
    qss.open( QFile::ReadOnly );
    QString ss = QString::fromLocal8Bit( qss.readAll() );
    qss.close();

    return ss;
};

QtGreet::QtGreet() {

	sett = new QSettings( "QtGreet", "QtGreet" );

	setFixedSize( qApp->primaryScreen()->size() );
    createUI();

	setStyleSheet( getStyleSheet( sett->value( "StyleSheet" ).toString() ) );

	if ( sett->contains( "VideoBG" ) ) {
		QStringList vbg = sett->value( "VideoBG" ).toString().split( "\\s+" );
		QProcess::startDetached( vbg.takeFirst(), vbg );
	}

	else {
		QString bgStr( sett->value( "Background" ).toString() );
	    background = QImage( bgStr ).scaled( size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );
	}
};

void QtGreet::createUI() {

    LayoutManager lytMgr( sett->value( "LayoutFile" ).toString() );

    QWidget *w = new QWidget();
    lytMgr.applyLayout( w );

    setCentralWidget( w );

	QMetaObject::connectSlotsByName( this );
};

void QtGreet::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );

	/* Base color */
    painter.save();
    painter.setPen( Qt::NoPen );
    painter.setBrush( QColor( "#00000060" ) );
    painter.drawRect( rect() );
    painter.restore();

	painter.save();
	painter.setOpacity( 0.5 );
	painter.drawImage( QPointF( 0, 0 ), background );
	painter.setOpacity( 1.0 );
	painter.restore();

    painter.end();

    QMainWindow::paintEvent( pEvent );
};

void QtGreet::keyPressEvent( QKeyEvent *kEvent ) {

    switch ( kEvent->key() ) {
        case Qt::Key_CapsLock : {
			LockState *lock = findChild<LockState *>( "CapsLock" );
			if ( not lock )
				return;

            if ( lock->objectName() == "CapsLock" )
                lock->toggle();

            break;
        }

        case Qt::Key_NumLock : {
			LockState *lock = findChild<LockState *>( "NumLock" );
			if ( not lock )
				return;

            if ( lock->objectName() == "NumLock" )
                lock->toggle();

            break;
        }

        default:
            return;
    }

    return;
};

void QtGreet::on_UserNavRight_clicked() {

	UserNameButton *btn = findChild<UserNameButton *>( "UserName" );
	if ( btn ) {
		btn->switchToNextUser();

		User usr = btn->currentUser();
		int uid = usr.uid;

		/* Update the 'UserIcon' */
		UserIcon *ui = findChild<UserIcon *>( "UserIcon" );
		if ( ui )
			ui->setPixmap( usr.icon );

		/* Update the 'UserName' label */
		UserNameLabel *un = findChild<UserNameLabel *>( "UserName" );
		if ( un ) {
			un->setText( usr.name );
		}

		/* Clear the password field */
		QLineEdit *pwd = findChild<QLineEdit *>( "Password" );
		if ( pwd )
			pwd->clear();
	}
};

void QtGreet::on_UserNavLeft_clicked() {

	UserNameButton *btn = findChild<UserNameButton *>( "UserName" );
	if ( btn ) {
		btn->switchToPreviousUser();

		User usr = btn->currentUser();
		int uid = usr.uid;

		/* Update the 'UserIcon' */
		UserIcon *ui = findChild<UserIcon *>( "UserIcon" );
		if ( ui )
			ui->setPixmap( usr.icon );

		/* Update the 'UserName' label */
		UserNameLabel *un = findChild<UserNameLabel *>( "UserName" );
		if ( un ) {
			un->setText( usr.name );
		}

		/* Clear the password field */
		QLineEdit *pwd = findChild<QLineEdit *>( "Password" );
		if ( pwd )
			pwd->clear();
	}
};

void QtGreet::on_SessionNavRight_clicked() {

	SessionNameButton *btn = findChild<SessionNameButton *>( "SessionName" );
	if ( btn )
		btn->switchToNextSession();
};

void QtGreet::on_SessionNavLeft_clicked() {

	SessionNameButton *btn = findChild<SessionNameButton *>( "SessionName" );
	if ( btn )
		btn->switchToPreviousSession();
};

void QtGreet::on_SessionEditButton_clicked() {
};

void QtGreet::on_LoginButton_clicked() {
};

void QtGreet::on_SessionName_clicked() {
};

void QtGreet::on_SessionName_curremtIndexChanged( int ) {
};

void QtGreet::on_SessionName_currentItemChanged( QListWidgetItem *cur, QListWidgetItem *old ) {

	Session sess( cur->data( Qt::UserRole + 1 ).value<Session>() );
};

void QtGreet::on_UserName_clicked() {

	// Show the user dialog!!
};

void QtGreet::on_UserName_curremtIndexChanged( int ) {

	// Reset the password and change the login session
};

void QtGreet::on_UserName_currentItemChanged( QListWidgetItem *cur, QListWidgetItem *old ) {

	User usr( cur->data( Qt::UserRole + 1 ).value<User>() );
	int uid = usr.uid;

	/* Update the 'UserName' label */
	UserNameLabel *un = findChild<UserNameLabel *>( "UserName" );
	if ( un ) {
		un->setText( usr.name );
	}

	/* Update the 'UserIcon' */
	UserIcon *ui = findChild<UserIcon *>( "UserIcon" );
	if ( ui ) {
		ui->setPixmap( usr.icon );
	}

	/* Clear the password field */
	QLineEdit *pwd = findChild<QLineEdit *>( "Password" );
	if ( pwd )
		pwd->clear();
};

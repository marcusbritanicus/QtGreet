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

static inline QString getStyleSheet( QString fn ) {

    QFile qss( fn );
    qss.open( QFile::ReadOnly );
    QString ss = QString::fromLocal8Bit( qss.readAll() );
    qss.close();

    return ss;
};

QtGreet::UI::UI() {

	setFixedSize( qApp->primaryScreen()->size() );
    createUI();

	setStyleSheet( getStyleSheet( qApp->arguments()[ 1 ].replace( "layout.hjson", "style.qss" ) ) );

	if ( sett->contains( "VideoBG" ) ) {
		QStringList vbg = sett->value( "VideoBG" ).toString().split( "\\s+" );
		QProcess::startDetached( vbg.takeFirst(), vbg );
	}

	else {
		QString bgStr( sett->value( "Background" ).toString() );
	    background = QImage( bgStr ).scaled( size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );
		if ( sett->value( "BlurBackground" ).toBool() ) {
			QPixmap img = QPixmap::fromImage( background );
			QGraphicsPixmapItem item( img );

			QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
			blur->setBlurRadius( 5.0 );
			item.setGraphicsEffect( blur );

			QGraphicsScene *scene = new QGraphicsScene();
			scene->addItem( &item );

			background.fill( Qt::transparent );
			QPainter p( &background );
			scene->render( &p, rect(), rect() );
		}
	}

	prepareUIforUse();
};

void QtGreet::UI::createUI() {

    QtGreet::LayoutManager lytMgr;
	QBoxLayout *lyt = lytMgr.generateLayout( qApp->arguments()[ 1 ] );

    QWidget *w = new QWidget();
	w->setLayout( lyt );

    setCentralWidget( w );

	QMetaObject::connectSlotsByName( this );
};

void QtGreet::UI::prepareUIforUse() {

	UserCombo *ulc = findChild<UserCombo *>( "UserCombo" );
	if ( ulc ) {
		User usr( ulc->currentUser() );
		updateUser( usr );
		updateSession( usr.uid );
	}

	UserList *ul = findChild<UserList *>( "UserList" );
	if ( ul ) {
		User usr( ul->currentUser() );
		updateUser( usr );
		updateSession( usr.uid );
	}

	UserLabel *unl = findChild<UserLabel *>( "UserLabel" );
	if ( unl ) {
		User usr( unl->currentUser() );
		updateUser( usr );
		updateSession( usr.uid );
	}
};

void QtGreet::UI::updateUser( User usr ) {

	/* Update the 'UserLabel' */
	UserLabel *un = findChild<UserLabel *>( "UserLabel" );
	if ( un )
		un->setText( QString( "%1 (%2)" ).arg( usr.name ).arg( usr.username ) );

	/* Update the 'UserIcon' */
	UserIcon *ui = findChild<UserIcon *>( "UserIcon" );
	if ( ui )
		ui->setPixmap( usr.icon );

	/* Clear the password field */
	QLineEdit *pwd = findChild<QLineEdit *>( "Password" );
	if ( pwd )
		pwd->clear();

	/* Session updates */
	updateSession( usr.uid );
};

void QtGreet::UI::updateSession( uint uid ) {

	QString sessFile = users->value( QString( "LastUsed/%1" ).arg( uid ) ).toString();
	Session curSess;
	if ( sessFile.isEmpty() ) {
		SessionName sessnm( true );
		curSess = sessnm.sessions().at( 0 );
		sessFile = curSess.file;
	}

	SessionListCombo *slc = findChild<SessionListCombo *>( "SessionName" );
	if ( slc ) {
		for( int i = 0; i < slc->sessions().count(); i++ ) {
			Session sess = slc->sessions().at( i );
			if ( sess.file == sessFile ) {
				curSess = sess;
				slc->setCurrentIndex( i );
				break;
			}
		}
	}

	SessionNameButton *snb = findChild<SessionNameButton *>( "SessionName" );
	if ( snb ) {
		for( int i = 0; i < snb->sessions().count(); i++ ) {
			Session sess = snb->sessions().at( i );
			if ( sess.file == sessFile ) {
				curSess = sess;
				snb->setText( sess.name );
				break;
			}
		}
	}

	SessionNameLabel *snl = findChild<SessionNameLabel *>( "SessionName" );
	if ( snl ) {
		for( int i = 0; i < snl->sessions().count(); i++ ) {
			Session sess = snl->sessions().at( i );
			if ( sess.file == sessFile ) {
				curSess = sess;
				snl->setText( sess.name );
				break;
			}
		}
	}

	SessionList *sl  = findChild<SessionList *>( "SessionName" );
	if ( sl ) {
		for( int i = 0; i < sl->sessions().count(); i++ ) {
			Session sess = sl->sessions().at( i );
			if ( sess.file == sessFile ) {
				curSess = sess;
				sl->setCurrentRow( i );
				break;
			}
		}
	}

	SessionEdit *se = findChild<SessionEdit *>( "SessionEdit" );
	if ( se )
		se->setText( curSess.exec );
};

void QtGreet::UI::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );

	/* Base color */

	painter.save();
	painter.drawImage( QPointF( 0, 0 ), background );
	painter.setOpacity( 1.0 );
	painter.restore();

    painter.save();
    painter.setPen( Qt::NoPen );
    painter.setBrush( QColor( 0, 0, 0, 150 ) );
    painter.drawRect( rect() );
    painter.restore();

    painter.end();

    QMainWindow::paintEvent( pEvent );
};

void QtGreet::UI::keyPressEvent( QKeyEvent *kEvent ) {

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

/* Auto Slots */

void QtGreet::UI::on_UserNavRight_clicked() {

	UserCombo *ulc = findChild<UserCombo *>( "UserCombo" );
	if ( ulc ) {
		ulc->switchToNextUser();
		updateUser( ulc->currentUser() );
		updateSession( ulc->currentUser().uid );
	}

	UserList *ul = findChild<UserList *>( "UserList" );
	if ( ul ) {
		ul->switchToNextUser();
		updateUser( ul->currentUser() );
		updateSession( ul->currentUser().uid );
	}

	UserLabel *unl = findChild<UserLabel *>( "UserLabel" );
	if ( unl ) {
		unl->switchToNextUser();
		updateUser( unl->currentUser() );
		updateSession( unl->currentUser().uid );
	}
};

void QtGreet::UI::on_UserNavLeft_clicked() {

	UserCombo *ulc = findChild<UserCombo *>( "UserCombo" );
	if ( ulc ) {
		ulc->switchToPreviousUser();
		updateUser( ulc->currentUser() );
		updateSession( ulc->currentUser().uid );
	}

	UserList *ul = findChild<UserList *>( "UserList" );
	if ( ul ) {
		ul->switchToPreviousUser();
		updateUser( ul->currentUser() );
		updateSession( ul->currentUser().uid );
	}

	UserLabel *unl = findChild<UserLabel *>( "UserLabel" );
	if ( unl ) {
		unl->switchToPreviousUser();
		updateUser( unl->currentUser() );
		updateSession( unl->currentUser().uid );
	}
};

void QtGreet::UI::on_SessionNavRight_clicked() {

	QString curSess;

	SessionListCombo *slc = findChild<SessionListCombo *>( "SessionName" );
	if ( slc ) {
		slc->switchToNextSession();
		curSess = slc->currentSession().exec;
	}

	SessionNameButton *snb = findChild<SessionNameButton *>( "SessionName" );
	if ( snb ) {
		snb->switchToNextSession();
		curSess = snb->currentSession().exec;
	}

	SessionNameLabel *snl = findChild<SessionNameLabel *>( "SessionName" );
	if ( snl ) {
		snl->switchToNextSession();
		curSess = snl->currentSession().exec;
	}

	SessionList *sl  = findChild<SessionList *>( "SessionName" );
	if ( sl ) {
		sl->switchToNextSession();
		curSess = sl->currentSession().exec;
	}

	SessionEdit *se = findChild<SessionEdit *>( "SessionEdit" );
	if ( se )
		se->setText( curSess );
};

void QtGreet::UI::on_SessionNavLeft_clicked() {

	QString curSess;

	SessionListCombo *slc = findChild<SessionListCombo *>( "SessionName" );
	if ( slc ) {
		slc->switchToPreviousSession();
		curSess = slc->currentSession().exec;
	}

	SessionNameButton *snb = findChild<SessionNameButton *>( "SessionName" );
	if ( snb ) {
		snb->switchToPreviousSession();
		curSess = snb->currentSession().exec;
	}

	SessionNameLabel *snl = findChild<SessionNameLabel *>( "SessionName" );
	if ( snl ) {
		snl->switchToPreviousSession();
		curSess = snl->currentSession().exec;
	}

	SessionList *sl  = findChild<SessionList *>( "SessionName" );
	if ( sl ) {
		sl->switchToPreviousSession();
		curSess = sl->currentSession().exec;
	}

	SessionEdit *se = findChild<SessionEdit *>( "SessionEdit" );
	if ( se )
		se->setText( curSess );
};

void QtGreet::UI::on_SessionEditButton_clicked() {
};

void QtGreet::UI::on_LoginButton_clicked() {
};

void QtGreet::UI::on_SessionName_clicked() {
};

void QtGreet::UI::on_SessionName_currentIndexChanged( int ) {
};

void QtGreet::UI::on_SessionName_currentItemChanged( QListWidgetItem *cur, QListWidgetItem *old ) {

	Session sess( cur->data( Qt::UserRole + 1 ).value<Session>() );
};

void QtGreet::UI::on_UserCombo_currentIndexChanged( int idx ) {

	// Reset the password and change the login session
	UserCombo *ulc = findChild<UserCombo *>( "UserCombo" );
	if ( not ulc )
		return;

	User usr( ulc->users().at( idx ) );
	updateUser( usr );
};

void QtGreet::UI::on_UserList_currentItemChanged( QListWidgetItem *cur, QListWidgetItem *old ) {

	User usr( cur->data( Qt::UserRole + 1 ).value<User>() );

	/* Update the 'UserName' label */
	UserLabel *un = findChild<UserLabel *>( "UserLabel" );
	if ( un )
		un->setText( QString( "%1 (%2)" ).arg( usr.name ).arg( usr.username ) );

	/* Update the 'UserIcon' */
	UserIcon *ui = findChild<UserIcon *>( "UserIcon" );
	if ( ui )
		ui->setPixmap( usr.icon );

	/* Clear the password field */
	QLineEdit *pwd = findChild<QLineEdit *>( "Password" );
	if ( pwd )
		pwd->clear();
};

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

#include "QtGreet.hpp"
#include "LoginManager.hpp"
#include "GreetdManager.hpp"

#include "LayoutManager.hpp"

#include "buttons.hpp"
#include "labels.hpp"
#include "power.hpp"
#include "session.hpp"
#include "user.hpp"

#include <unistd.h>
#include <QtDBus>

QtGreet::UI::UI() {
    login = new GreetDLogin();

    themeManager = new ThemeManager( sett->value( "Theme", "default" ).toString() );

    setFixedSize( qApp->primaryScreen()->size() );
    createUI();

    setStyleSheet( themeManager->getStyleSheet() );

    if ( sett->contains( "VideoBG" ) ) {
        QStringList vbg = sett->value( "VideoBG" ).toString().split( "\\s+" );
        QProcess::startDetached( vbg.takeFirst(), vbg );
    }

    else {
        QString bgStr( themeManager->background() );
        background = QImage( bgStr ).scaled( size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );

        if ( sett->value( "BlurBackground", false ).toBool() ) {
            QPixmap             img = QPixmap::fromImage( background );
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

    QAction *act = new QAction( "Quit" );
    act->setShortcut( QKeySequence( Qt::SHIFT | Qt::CTRL | Qt::Key_Q ) );
    connect( act, &QAction::triggered, [=]() {
        qInfo() << "Quitting";
        qApp->quit();
    } );

    addAction( act );
}


void QtGreet::UI::createUI() {
    base = new QStackedWidget();
    setCentralWidget( base );

    QtGreet::LayoutManager lytMgr;

    QWidget    *w   = new QWidget( base );
    QBoxLayout *lyt = lytMgr.generateLayout( themeManager->getLayout() );

    w->setLayout( lyt );

    base->addWidget( w );

    setWindowFlags( Qt::FramelessWindowHint );

    QMetaObject::connectSlotsByName( this );
}


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
}


void QtGreet::UI::updateUser( User usr ) {
    mCurUser = usr;

    /* Update the 'UserLabel' */
    UserLabel *un = findChild<UserLabel *>( "UserLabel" );

    if ( un ) {
        un->setText( QString( "%1 (%2)" ).arg( usr.name ).arg( usr.username ) );
    }

    /* Update the 'UserIcon' */
    UserIcon *ui = findChild<UserIcon *>( "UserIcon" );

    if ( ui ) {
        ui->setPixmap( usr.icon );
    }

    /* Clear the password field */
    QLineEdit *pwd = findChild<QLineEdit *>( "Password" );

    if ( pwd ) {
        pwd->clear();
    }

    /* Session updates */
    updateSession( usr.uid );
}


void QtGreet::UI::updateSession( uint uid ) {
    QString sessFile = users->value( QString( "LastUsed/%1" ).arg( uid ) ).toString();
    Session curSess;

    if ( sessFile.isEmpty() ) {
        SessionName sessnm( true );
        curSess  = sessnm.sessions().at( 0 );
        sessFile = curSess.file;
    }

    SessionCombo *slc = findChild<SessionCombo *>( "SessionCombo" );

    if ( slc ) {
        for ( int i = 0; i < slc->sessions().count(); i++ ) {
            Session sess = slc->sessions().at( i );

            if ( sess.file == sessFile ) {
                curSess = sess;
                slc->setCurrentIndex( i );
                break;
            }
        }
    }

    SessionLabel *snl = findChild<SessionLabel *>( "SessionLabel" );

    if ( snl ) {
        for ( int i = 0; i < snl->sessions().count(); i++ ) {
            Session sess = snl->sessions().at( i );

            if ( sess.file == sessFile ) {
                curSess = sess;
                snl->setText( sess.name );
                break;
            }
        }
    }

    SessionList *sl = findChild<SessionList *>( "SessionList" );

    if ( sl ) {
        for ( int i = 0; i < sl->sessions().count(); i++ ) {
            Session sess = sl->sessions().at( i );

            if ( sess.file == sessFile ) {
                curSess = sess;
                sl->setCurrentRow( i );
                break;
            }
        }
    }

    SessionEdit *se = findChild<SessionEdit *>( "SessionEdit" );

    if ( se ) {
        se->setText( curSess.exec );
    }

    mCurSession = curSess;
}


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
}


void QtGreet::UI::keyPressEvent( QKeyEvent *kEvent ) {
    switch ( kEvent->key() ) {
        case Qt::Key_CapsLock: {
            LockState *lock = findChild<LockState *>( "CapsLock" );

            if ( not lock ) {
                return;
            }

            if ( lock->objectName() == "CapsLock" ) {
                lock->toggle();
            }

            break;
        }

        case Qt::Key_NumLock: {
            LockState *lock = findChild<LockState *>( "NumLock" );

            if ( not lock ) {
                return;
            }

            if ( lock->objectName() == "NumLock" ) {
                lock->toggle();
            }

            break;
        }

        default: {
            return;
        }
    }

    return;
}


void QtGreet::UI::tryLogin() {
    /** Save the user info */
    users->setValue( QString( "LastUsed/%1" ).arg( mCurUser.uid ), mCurSession.file );

    QLineEdit *pwd = findChild<QLineEdit *>( "Password" );

    if ( not pwd ) {
        return;
    }

    setDisabled( true );
    bool auth = login->authenticate( mCurUser.username, pwd->text() );
    bool sess = false;

    if ( auth ) {
        sess = login->startSession( mCurSession.exec, mCurSession.type );

        if ( sess ) {
            qApp->quit();
        }
    }

    QString errTitle;
    QString errMsg;

    if ( not auth ) {
        errTitle = "Authentication failure";
        errMsg   = "We failed to authenticate you. Did you enter the correct password?";
    }

    else {
        errTitle = "Failed to start Session";
        errMsg   = "We failed to start the selected session. Perhaps a wrong command?";
    }

    QMessageBox::critical(
        this,
        "QtGreet | Failure",
        QString( "<b>%1</b><p>%2</p>" ).arg( errTitle ).arg( errMsg ),
        QMessageBox::Ok
    );

    setEnabled( true );

    if ( not auth ) {
        pwd->selectAll();
        pwd->setFocus();
    }
}


/* Auto Slots */

void QtGreet::UI::on_UserNavRight_clicked() {
    qDebug() << "Loading next user";

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
}


void QtGreet::UI::on_UserNavLeft_clicked() {
    qDebug() << "Loading prev user";

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
}


void QtGreet::UI::on_SessionNavRight_clicked() {
    QString curSess;

    SessionCombo *slc = findChild<SessionCombo *>( "SessionCombo" );

    if ( slc ) {
        slc->switchToNextSession();
        curSess     = slc->currentSession().exec;
        mCurSession = slc->currentSession();
    }

    SessionLabel *snl = findChild<SessionLabel *>( "SessionLabel" );

    if ( snl ) {
        snl->switchToNextSession();
        curSess     = snl->currentSession().exec;
        mCurSession = snl->currentSession();
    }

    SessionList *sl = findChild<SessionList *>( "SessionList" );

    if ( sl ) {
        sl->switchToNextSession();
        curSess     = sl->currentSession().exec;
        mCurSession = sl->currentSession();
    }

    SessionEdit *se = findChild<SessionEdit *>( "SessionEdit" );

    if ( se ) {
        se->setText( curSess );
    }
}


void QtGreet::UI::on_SessionNavLeft_clicked() {
    QString curSess;

    SessionCombo *slc = findChild<SessionCombo *>( "SessionCombo" );

    if ( slc ) {
        slc->switchToPreviousSession();
        curSess     = slc->currentSession().exec;
        mCurSession = slc->currentSession();
    }

    SessionLabel *snl = findChild<SessionLabel *>( "SessionLabel" );

    if ( snl ) {
        snl->switchToPreviousSession();
        curSess     = snl->currentSession().exec;
        mCurSession = snl->currentSession();
    }

    SessionList *sl = findChild<SessionList *>( "SessionList" );

    if ( sl ) {
        sl->switchToPreviousSession();
        curSess     = sl->currentSession().exec;
        mCurSession = sl->currentSession();
    }

    SessionEdit *se = findChild<SessionEdit *>( "SessionEdit" );

    if ( se ) {
        se->setText( curSess );
    }
}


void QtGreet::UI::on_SessionEditButton_clicked() {
}


void QtGreet::UI::on_LoginButton_clicked() {
    tryLogin();
}


void QtGreet::UI::on_Password_returnPressed() {
    tryLogin();
}


void QtGreet::UI::on_SessionEdit_returnPressed() {
    tryLogin();
}


void QtGreet::UI::on_SessionCombo_currentIndexChanged( int ) {
}


void QtGreet::UI::on_SessionList_currentItemChanged( QListWidgetItem *cur, QListWidgetItem * ) {
    Session sess( cur->data( Qt::UserRole + 1 ).value<Session>() );
}


void QtGreet::UI::on_UserCombo_currentIndexChanged( int idx ) {
    // Reset the password and change the login session
    UserCombo *ulc = findChild<UserCombo *>( "UserCombo" );

    if ( not ulc ) {
        return;
    }

    User usr( ulc->users().at( idx ) );

    updateUser( usr );
}


void QtGreet::UI::on_UserList_currentItemChanged( QListWidgetItem *cur, QListWidgetItem * ) {
    User usr( cur->data( Qt::UserRole + 1 ).value<User>() );

    /* Update the 'UserName' label */
    UserLabel *un = findChild<UserLabel *>( "UserLabel" );

    if ( un ) {
        un->setText( QString( "%1 (%2)" ).arg( usr.name ).arg( usr.username ) );
    }

    /* Update the 'UserIcon' */
    UserIcon *ui = findChild<UserIcon *>( "UserIcon" );

    if ( ui ) {
        ui->setPixmap( usr.icon );
    }

    /* Clear the password field */
    QLineEdit *pwd = findChild<QLineEdit *>( "Password" );

    if ( pwd ) {
        pwd->clear();
    }
}

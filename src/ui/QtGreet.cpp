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

#include "ThemeManager.hpp"
#include "LayoutManager.hpp"

#include "buttons.hpp"
#include "labels.hpp"
#include "power.hpp"
#include "session.hpp"
#include "user.hpp"
#include "MpvWidget.hpp"

#include <unistd.h>
#include <QtDBus>

QtGreet::UI::UI( QScreen *scrn ) {
    /** We need this later on... */
    mScreen = scrn;

    /** Backend */
    login = new GreetdLogin();

    /** To shutdown/reboot */
    login1 = new DFL::Login1( this );

    /**
     * The option passed as cli will take precedence.
     * Assume that the user knows the theme names.
     */
    themeMgr = new QtGreet::ThemeManager( themeName.length() ? themeName : sett->value( "Theme", "default" ).toString() );

    /** Fix the size of the window */
    setFixedSize( scrn->size() );

    /** Set the window title */
    setWindowTitle( QString( "QtGreet-%1-%2x%3" ).arg( scrn->name() ).arg( scrn->size().width() ).arg( scrn->size().height() ) );

    createUI();

    if ( themeMgr->isVideoBG() == false ) {
        QString bgStr( themeMgr->background() );
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
            p.end();
        }
    }

    prepareUIforUse();

    QAction *act = new QAction( "Quit" );

    act->setShortcut( QKeySequence( Qt::SHIFT | Qt::CTRL | Qt::Key_Q ) );
    connect(
        act, &QAction::triggered, [ = ]() {
            qInfo() << "Quitting";
            qApp->quit();
        }
    );

    addAction( act );

    QLineEdit *pwd = findChild<QLineEdit *>( "Password" );

    if ( pwd ) {
        pwd->setFocus();
    }
}


QtGreet::UI::~UI() {
    delete themeMgr;
    delete lytMgr;
    delete base;
    delete login;
    delete login1;
    delete validating;
    delete cLogin;
    delete mScreen;
}


void QtGreet::UI::createUI() {
    base = new QStackedWidget();
    setCentralWidget( base );

    lytMgr = new QtGreet::LayoutManager( mScreen );

    QWidget *w = new QWidget( base );

    /** The widget layout */
    QBoxLayout *themeLyt = lytMgr->generateLayout( themeMgr->getLayout() );

    /** Base layout on which the themeLyt will be added */
    QGridLayout *baseLyt = new QGridLayout();
    baseLyt->setContentsMargins( QMargins() );

    if ( themeMgr->isVideoBG() ) {
        setlocale( LC_NUMERIC, "C" );
        MpvWidget *video = new MpvWidget( w );
        video->command( QStringList() << "loadfile" << themeMgr->video() );

        baseLyt->addWidget( video, 0, 0 );
    }

    baseLyt->addLayout( themeLyt, 0, 0 );
    w->setLayout( baseLyt );

    base->addWidget( w );

    setWindowFlags( Qt::FramelessWindowHint );

    setStyleSheet( themeMgr->getStyleSheet() );

    QMetaObject::connectSlotsByName( this );

    validating = new QLabel();
    validating->close();

    /** Create custom login widget */
    cLogin = new QWidget( this );
    cLogin->setObjectName( "OverLay" );
    cLogin->setLayout( lytMgr->generateLayout( ":/customlogin.hjson" ) );

    /* Set the Custom login stylesheet */
    QFile qss( ":/customlogin.qss" );
    qss.open( QFile::ReadOnly );
    cLogin->setStyleSheet( QString::fromLocal8Bit( qss.readAll() ) );
    qss.close();

    /** Hide it for now */
    cLogin->hide();

    QAction *cstmLgnAct = new QAction( "Show custom login" );
    cstmLgnAct->setShortcut( QKeySequence( Qt::CTRL | Qt::SHIFT | Qt::Key_X ) );

    connect(
        cstmLgnAct, &QAction::triggered, [ = ] () {
            showCustomLogin();
        }
    );

    addAction( cstmLgnAct );
}


void QtGreet::UI::prepareUIforUse() {
    int lastUsedId = users->value( "LastUsedId", -1 ).toInt();

    UserCombo *ulc = findChild<UserCombo *>( "UserCombo" );

    if ( ulc ) {
        /** Update the widget with the last used user id */
        if ( lastUsedId != -1 ) {
            ulc->setCurrentUser( (uint)lastUsedId );
        }

        /** Get the updated User */
        User usr( ulc->currentUser() );
        updateUser( usr );
        updateSession( usr.uid );
    }

    UserList *ul = findChild<UserList *>( "UserList" );

    if ( ul ) {
        /** Update the widget with the last used user id */
        if ( lastUsedId != -1 ) {
            ul->setCurrentUser( (uint)lastUsedId );
        }

        /** Get the updated User */
        User usr( ul->currentUser() );
        updateUser( usr );
        updateSession( usr.uid );
    }

    UserLabel *unl = findChild<UserLabel *>( "UserLabel" );

    if ( unl ) {
        /** Update the widget with the last used user id */
        if ( lastUsedId != -1 ) {
            unl->setCurrentUser( (uint)lastUsedId );
        }

        /** Get the updated User */
        User usr( unl->currentUser() );
        updateUser( usr );
        updateSession( usr.uid );
    }

    UserEdit *une = findChild<UserEdit *>( "UserEdit" );

    if ( une ) {
        /** Update the widget with the last used user id */
        if ( lastUsedId != -1 ) {
            une->setCurrentUser( (uint)lastUsedId );
        }

        /** Get the updated User */
        User usr( une->currentUser() );
        updateUser( usr );
        updateSession( usr.uid );
    }
}


void QtGreet::UI::updateUser( User usr ) {
    mCurUser = usr;

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
    /** We need to paint only if it's image background */
    if ( themeMgr->isVideoBG() == false ) {
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
    }

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


void QtGreet::UI::showValidating() {
    QWidget *base = new QWidget( this );

    base->setObjectName( "OverLay" );
    base->setFixedSize( size() );

    QWidget *msg = new QWidget();

    msg->setObjectName( "widget" );

    validating->setText( "<b>Validating...</b>" );

    QPushButton *btn = new QPushButton( QIcon( ":/icons/arrow-left.png" ), "&Back" );

    btn->setFixedSize( 100, 36 );
    connect( btn, &QPushButton::clicked, base, &QWidget::close );
    connect(
        btn, &QPushButton::clicked, [ = ] () {
            base->close();
            QLineEdit *pwd = findChild<QLineEdit *>( "Password" );

            if ( pwd ) {
                pwd->selectAll();
                pwd->setFocus();
            }
        }
    );

    QGridLayout *lyt = new QGridLayout();

    lyt->setContentsMargins( QMargins( 100, 100, 100, 100 ) );
    lyt->setSpacing( 50 );
    lyt->addWidget( validating, 0, 0, Qt::AlignCenter );
    lyt->addWidget( btn,        1, 0, Qt::AlignCenter );

    msg->setLayout( lyt );

    QGridLayout *blyt = new QGridLayout();

    blyt->addWidget( msg, 0, 0, Qt::AlignCenter );

    base->setLayout( blyt );
    base->setStyleSheet(
        "#widget {"
        "    border: 2px solid gray;"
        "    border-radius: 5px;"
        "}"
        "QPushButton {"
        "    border: 2px solid gray;"
        "    border-radius: 10px;"
        "    background-color: rgba(255, 255, 255, 30);"
        "}"
    );

    base->show();
    btn->setFocus();
}


void QtGreet::UI::showCustomLogin() {
    /** Connect the back button to close the window */
    QToolButton *backBtn = cLogin->findChild<QToolButton *>( "BackButton" );

    connect(
        backBtn, &QToolButton::clicked, [ = ] () {
            cLogin->close();
            backBtn->disconnect();
        }
    );

    /** Login Button */
    QPushButton *loginBtn = cLogin->findChild<QPushButton *>( "CustomLogin" );

    connect(
        loginBtn, &QToolButton::clicked, [ = ] () {
            tryCustomLogin();
        }
    );

    QLineEdit *le = cLogin->findChild<QLineEdit *>( "UserEdit" );

    le->setFocus();

    cLogin->resize( size() );
    cLogin->show();
}


void QtGreet::UI::showPowerMessage( bool reboot, bool done ) {
    QWidget *base = new QWidget( this );

    base->setObjectName( "OverLay" );
    base->setFixedSize( size() );

    QWidget *msg      = new QWidget();
    QLabel  *powerMsg = new QLabel();

    msg->setObjectName( "widget" );

    QString message1(
        "<b>%1</b><br>"
        "<p>"
        "Your system will be %2 shortly. Please standby."
        "</p>"
    );

    QString message2(
        "<b>%1</b><br>"
        "<p>"
        "Your system will be %2 in %3s. Click <tt>[Cancel]</tt> to cancel."
        "</p>"
    );

    if ( done ) {
        powerMsg->setText( message1.arg( reboot ? "Rebooting..." : "Shutting Down..." ).arg( reboot ? "reboot" : "shutdown" ) );
    }

    else {
        powerMsg->setText( message2.arg( reboot ? "Rebooting..." : "Shutting Down..." ).arg( reboot ? "reboot" : "shutdown" ).arg( 10 ) );
    }

    QTimer *timer = new QTimer();

    timer->setInterval( 1000 );
    timer->setTimerType( Qt::CoarseTimer );

    QPushButton *btn = new QPushButton( QIcon::fromTheme( "dialog-close" ), "&Cancel" );

    btn->setToolTip( QString( "Cancel the %1" ).arg( reboot ? "reboot" : "shutdown" ) );
    btn->setFixedSize( 100, 36 );

    connect(
        btn, &QPushButton::clicked, [ = ] () {
            timer->stop();
            login1->schedulePowerEvent( "cancel", 0 );
            base->close();
        }
    );

    QGridLayout *lyt = new QGridLayout();

    lyt->setContentsMargins( QMargins( 100, 100, 100, 100 ) );
    lyt->setSpacing( 50 );
    lyt->addWidget( powerMsg, 0, 0, Qt::AlignCenter );

    if ( !done ) {
        lyt->addWidget( btn, 1, 0, Qt::AlignCenter );
    }

    msg->setLayout( lyt );

    QGridLayout *blyt = new QGridLayout();

    blyt->addWidget( msg, 0, 0, Qt::AlignCenter );

    base->setLayout( blyt );
    base->setStyleSheet(
        "#widget {"
        "    border: 2px solid gray;"
        "    border-radius: 5px;"
        "}"
        "QPushButton {"
        "    border: 2px solid gray;"
        "    border-radius: 10px;"
        "    background-color: rgba(255, 255, 255, 30);"
        "}"
    );

    base->show();
    btn->setFocus();

    int remaining = 10;

    connect(
        timer, &QTimer::timeout, [ = ] () mutable {
            remaining--;

            /**
             * 10s are up. Close the GUI.
             * Shutdown will be in progress.
             */
            if ( remaining <= 0 ) {
                timer->stop();
                qApp->quit();
            }

            else {
                if ( done ) {
                    powerMsg->setText( message1.arg( reboot ? "Rebooting..." : "Shutting Down..." ).arg( reboot ? "reboot" : "shutdown" ) );
                }
                else {
                    powerMsg->setText( message2.arg( reboot ? "Rebooting..." : "Shutting Down..." ).arg( reboot ? "reboot" : "shutdown" ).arg( remaining ) );
                }
            }
        }
    );

    login1->schedulePowerEventIn( (reboot ? "reboot" : "poweroff"), 10 * 1000 * 1000 );

    timer->start();
}


void QtGreet::UI::tryLogin() {
    QLineEdit *pwd = findChild<QLineEdit *>( "Password" );

    /** If we could not get the password widget, do nothing. */
    if ( pwd == nullptr ) {
        return;
    }

    /** If the current user name is invalid, do nothing */
    if ( mCurUser.username.length() == 0 ) {
        return;
    }

    showValidating();

    setDisabled( true );

    bool    auth    = login->authenticate( mCurUser.username, pwd->text() );
    QString authMsg = login->errorMessage();
    bool    sess    = false;
    QString sessMsg;

    if ( auth ) {
        sess    = login->startSession( mCurSession.exec, mCurSession.type );
        sessMsg = login->errorMessage();

        if ( sess ) {
            qDebug() << "Login successful";
            qDebug() << "Saving the session details.";
            /** Save the user info */
            users->setValue( "LastUsedId",                                 mCurUser.uid );
            users->setValue( QString( "LastUsed/%1" ).arg( mCurUser.uid ), mCurSession.file );
            users->sync();

            qApp->quit();
        }
    }

    QString errTitle;
    QString errMsg;

    if ( auth ) {
        errTitle = "Failed to start Session";
        errMsg   = "We failed to start the selected session. Perhaps a wrong command?";
        errMsg  += "<p>Server says <br><center><tt>" + sessMsg + "</tt></center></p>";
    }

    else {
        errTitle = "Authentication failure";
        errMsg   = "We failed to authenticate you. Did you enter the correct password?";
        errMsg  += "<p>Server says <br><center><tt>" + authMsg + "</tt></center></p>";
    }

    validating->setText( QString( "<center><b>%1</b></center><p>%2</p>" ).arg( errTitle ).arg( errMsg ) );

    setEnabled( true );
}


void QtGreet::UI::tryCustomLogin() {
    QLineEdit *usr = cLogin->findChild<QLineEdit *>( "UserEdit" );
    QLineEdit *pwd = cLogin->findChild<QLineEdit *>( "Password" );
    QLineEdit *ssn = cLogin->findChild<QLineEdit *>( "SessionEdit" );

    if ( (usr == nullptr) or (pwd == nullptr) or (ssn == nullptr) ) {
        return;
    }

    showValidating();

    setDisabled( true );

    bool    auth    = login->authenticate( usr->text(), pwd->text() );
    QString authMsg = login->errorMessage();
    bool    sess    = false;
    QString sessMsg;

    if ( auth ) {
        sess    = login->startSession( ssn->text(), "other" );
        sessMsg = login->errorMessage();

        if ( sess ) {
            qDebug() << "Login successful";
            qDebug() << "Saving the custom session details.";

            users->setValue( "Custom/User",    usr->text() );
            users->setValue( "Custom/Session", ssn->text() );
            users->sync();

            qApp->quit();
        }
    }

    QString errTitle;
    QString errMsg;

    if ( auth ) {
        errTitle = "Failed to start Session";
        errMsg   = "We failed to start the selected session. Perhaps a wrong command?";
        errMsg  += "<p>Server says <br><center><tt>" + sessMsg + "</tt></center></p>";
    }

    else {
        errTitle = "Authentication failure";
        errMsg   = "We failed to authenticate you. Did you enter the correct password?";
        errMsg  += "<p>Server says <br><center><tt>" + authMsg + "</tt></center></p>";
    }

    validating->setText( QString( "<center><b>%1</b></center><p>%2</p>" ).arg( errTitle ).arg( errMsg ) );

    setEnabled( true );
}


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
}


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
    UserCombo *ulc = findChild<UserCombo *>( "UserCombo" );

    if ( ulc ) {
        User usr( ulc->users().at( idx ) );

        updateUser( usr );
    }
}


void QtGreet::UI::on_UserEdit_textEdited( QString ) {
    UserEdit *ulc = findChild<UserEdit *>( "UserEdit" );

    if ( ulc ) {
        updateUser( ulc->currentUser() );
    }
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


void QtGreet::UI::on_PowerButton_suspend() {
    login1->request( "Suspend" );
}


void QtGreet::UI::on_PowerButton_hibernate() {
    login1->request( "Hibernate" );
}


void QtGreet::UI::on_PowerButton_shutdown( bool done ) {
    showPowerMessage( false, done );
}


void QtGreet::UI::on_PowerButton_reboot( bool done ) {
    showPowerMessage( true, done );
}


void QtGreet::UI::on_Suspend_suspend() {
    login1->request( "Suspend" );
}


void QtGreet::UI::on_Hibernate_hibernate() {
    login1->request( "Hibernate" );
}


void QtGreet::UI::on_Halt_shutdown( bool done ) {
    showPowerMessage( false, done );
}


void QtGreet::UI::on_Reboot_reboot( bool done ) {
    showPowerMessage( true, done );
}

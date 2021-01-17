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
#include "ClockWidget.hpp"

#include <unistd.h>

extern "C" {
    #include "proto.h"
}

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

static bool IsExec( QString exec ) {

    /* If this is a full path (ex: /usr/bin/fish) */
    if ( access( exec.toUtf8().constData(), R_OK | X_OK ) == 0 )
        return true;

    /* Otherwise (ex: plasmashell) */
    for( QString path : qgetenv( "PATH" ).split( ':' ) ) {
        int ret = access( QString( path + "/" + exec ).toUtf8().constData(), R_OK | X_OK );
        if ( ret == 0 )
            return true;
    }

    return false;
};

static QString getStyleSheet( QString textColor ) {

    QFile qss( ":/StyleSheet.qss" );
    qss.open( QFile::ReadOnly );
    QString ss = QString::fromLocal8Bit( qss.readAll() );
    qss.close();

    return ss.replace( "<text_color>", textColor );
};

QListWidget* getListWidget() {

    QListWidget *lw = new QListWidget();
    lw->setIconSize( QSize( 36, 36 ) );
    lw->setFont( QFont( "Quicksand", 12 ) );
    lw->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    lw->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    return lw;
};

QtGreet::QtGreet() {

    setFixedSize( qApp->primaryScreen()->size() );

    getUserList();
    getLoginSessions();

    createUI();

    setStyleSheet( getStyleSheet( textColor ) );

    userPass->setFocus();

    updateUser();
};

void QtGreet::createUI() {

    QIcon::setThemeName( "breeze-dark" );

    setWindowFlags( Qt::FramelessWindowHint );
    setAttribute( Qt::WA_TranslucentBackground );

    QSettings settings( "/etc/qtgreet/config.ini", QSettings::IniFormat );
    baseColor = QColor( "#" + settings.value( "BaseColor" ).toString() );

    textColor = settings.value( "TextColor", "ffffff" ).toString();

    QString bgStr( settings.value( "Background" ).toString() );
    if ( bgStr != "none" )
        background = QImage( bgStr ).scaled( size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );

    else
        drawBG = false;

    if ( getFormFactor() != 2 )
        ClockWidget * clock = new ClockWidget( QColor( "#" + textColor ), this );

    userConfig = new QSettings( "/etc/qtgreet/users.conf", QSettings::IniFormat );

    /* Layer 1 */
    userIcon = new QLabel();
    userIcon->setAlignment( Qt::AlignCenter );
    userIcon->setFixedSize( QSize( 72, 72 ) );
    userIcon->setStyleSheet( "border: 2px solid gray; border-radius: 6px;" );
    userIcon->setPixmap( QIcon( ":/icons/user.png" ).pixmap( QSize( 64, 64 ) ) );

    QHBoxLayout *lyr1Lyt = new QHBoxLayout();
    lyr1Lyt->addStretch();
    lyr1Lyt->addWidget( userIcon );
    lyr1Lyt->addStretch();

    /* Layer 2 */

    prevUBtn = new QToolButton();
    prevUBtn->setObjectName( "nav" );
    prevUBtn->setFixedSize( QSize( 36, 36 ) );
    prevUBtn->setIcon( QIcon( ":/icons/arrow-left.png" ) );
    prevUBtn->setAutoRaise( true );
    connect( prevUBtn, &QToolButton::clicked, this, &QtGreet::previousUser );

    userName = new QPushButton();
    userName->setFixedSize( QSize( 270, 36 ) );
    userName->setFont( QFont( "Quicksand", 12, QFont::Bold ) );
    connect( userName, &QPushButton::clicked, this, &QtGreet::showUserSelection );

    nextUBtn = new QToolButton();
    nextUBtn->setObjectName( "nav" );
    nextUBtn->setFixedSize( QSize( 36, 36 ) );
    nextUBtn->setIcon( QIcon( ":/icons/arrow-right.png" ) );
    nextUBtn->setAutoRaise( true );
    connect( nextUBtn, &QToolButton::clicked, this, &QtGreet::nextUser );

    QHBoxLayout *lyr2Lyt = new QHBoxLayout();
    lyr2Lyt->setSpacing( 10 );
    lyr2Lyt->addStretch();
    lyr2Lyt->addWidget( prevUBtn );
    lyr2Lyt->addWidget( userName );
    lyr2Lyt->addWidget( nextUBtn );
    lyr2Lyt->addStretch();

    /* Layer 3 */

    userPass = new QLineEdit();
    userPass->setAlignment( Qt::AlignCenter );
    userPass->setPlaceholderText( "Password" );
    userPass->setFixedSize( QSize( 270, 36 ) );
    userPass->setEchoMode( QLineEdit::Password );
    connect( userPass, &QLineEdit::returnPressed, this, &QtGreet::tryLogin );

    QHBoxLayout *lyr3Lyt = new QHBoxLayout();
    lyr3Lyt->addStretch();
    lyr3Lyt->addWidget( userPass );
    lyr3Lyt->addStretch();

    /* Layer 4 */

    loginBtn = new QPushButton( QIcon( ":/icons/arrow-right.png" ), "&Login" );
    loginBtn->setFixedSize( 100, 36 );
    connect( loginBtn, &QPushButton::clicked, this, &QtGreet::tryLogin );

    QHBoxLayout *lyr4Lyt = new QHBoxLayout();
    lyr4Lyt->addStretch();
    lyr4Lyt->addWidget( loginBtn );
    lyr4Lyt->addStretch();

    /* Layer 5 */

    prevSBtn = new QToolButton();
    prevSBtn->setObjectName( "nav" );
    prevSBtn->setFixedSize( QSize( 36, 36 ) );
    prevSBtn->setIcon( QIcon( ":/icons/arrow-left.png" ) );
    prevSBtn->setAutoRaise( true );
    connect( prevSBtn, &QToolButton::clicked, this, &QtGreet::previousSess );

    session = new QPushButton();
    session->setFixedSize( QSize( 270, 36 ) );
    session->setFont( QFont( "Quicksand", 12, QFont::Bold ) );
    connect( session, &QPushButton::clicked, this, &QtGreet::showSessionSelection );

    nextSBtn = new QToolButton();
    nextSBtn->setObjectName( "nav" );
    nextSBtn->setFixedSize( QSize( 36, 36 ) );
    nextSBtn->setIcon( QIcon( ":/icons/arrow-right.png" ) );
    nextSBtn->setAutoRaise( true );
    nextSBtn->setStyleSheet( "border: 1px solid gray; border-radius: 18px;" );
    connect( nextSBtn, &QToolButton::clicked, this, &QtGreet::nextSess );

    QHBoxLayout *lyr5Lyt = new QHBoxLayout();
    lyr5Lyt->setSpacing( 10 );
    lyr5Lyt->addStretch();
    lyr5Lyt->addWidget( prevSBtn );
    lyr5Lyt->addWidget( session );
    lyr5Lyt->addWidget( nextSBtn );
    lyr5Lyt->addStretch();

    /* Layer 6 */

    sessionCmd = new QLineEdit();
    sessionCmd->setFixedSize( QSize( 270, 36 ) );
    sessionCmd->setAlignment( Qt::AlignCenter );
    sessionCmd->setFont( QFont( "Quicksand", 12 ) );
    sessionCmd->setStyleSheet( "border: 1px solid gray; border-radius: 18px; padding-left: 10px; padding-right: 10px;" );
    updateSess();

    QHBoxLayout *lyr6Lyt = new QHBoxLayout();
    lyr6Lyt->addStretch();
    lyr6Lyt->addWidget( sessionCmd );
    lyr6Lyt->addStretch();

    /* Final Layout */
    QVBoxLayout *lyt = new QVBoxLayout();
    lyt->setContentsMargins( QMargins() );
    lyt->setSpacing( 20 );

    lyt->addStretch();

    /* UserIcon */
    lyt->addLayout( lyr1Lyt );

    lyt->addStretch();

    /* Prev / Name / Next */
    lyt->addLayout( lyr2Lyt );

    /* Password */
    lyt->addLayout( lyr3Lyt );

    lyt->addStretch();

    /* Login Btn */
    lyt->addLayout( lyr4Lyt );

    lyt->addStretch();

    /* Prev / DE / Next */
    lyt->addLayout( lyr5Lyt );
    lyt->addLayout( lyr6Lyt );

    lyt->addStretch();

    QWidget *base = new QWidget();
    base->setLayout( lyt );

    setCentralWidget( base );

    /* Build the User/Sessions dialog */
    overLay = new QWidget( this );
    overLay->setObjectName( "OverLay" );
    overLay->setFixedSize( size() );

    QWidget *w = new QWidget();
    w->setObjectName( "widget" );
    w->setFixedSize( 640, 480 );

    userLW = getListWidget();
    connect(
        userLW, &QListWidget::itemClicked, [this]( QListWidgetItem* item ) {
            curUser = item->data( Qt::UserRole + 1 ).toUInt();
            updateUser();

            overLay->close();
        }
    );

    sessionLW = getListWidget();
    connect(
        sessionLW, &QListWidget::itemClicked, [this]( QListWidgetItem* item ) {
            curSess = item->data( Qt::UserRole + 1 ).toUInt();
            updateSess();

            overLay->close();
        }
    );

    QPushButton *backBtn = new QPushButton( QIcon( ":/icons/arrow-left" ), "&Back" );
    backBtn->setFixedSize( QSize( 100, 36 ) );

    QHBoxLayout *blyt = new QHBoxLayout();
    blyt->addStretch();
    blyt->addWidget( backBtn );
    blyt->addStretch();

    connect( backBtn, &QPushButton::clicked, overLay, &QWidget::close );

    QVBoxLayout *wlyt = new QVBoxLayout();
    wlyt->setContentsMargins( QMargins( 20, 20, 20, 20 ) );
    wlyt->addWidget( userLW );
    wlyt->addWidget( sessionLW );
    wlyt->addLayout( blyt );

    w->setLayout( wlyt );

    QGridLayout *olyt = new QGridLayout();
    olyt->addWidget( w );
    overLay->setLayout( olyt );

    overLay->close();

    /* Validating message */
    validating = new QLabel();
    validating->close();
};

/* List the users on this system */
void QtGreet::getUserList() {

    /* Read /etc/login.defs */
    QFile loginDefs( "/etc/login.defs" );
    loginDefs.open( QFile::ReadOnly );
    QString defs = QString::fromLocal8Bit( loginDefs.readAll() );
    loginDefs.close();

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
                uInfo.value( 6 )
            };

            mUserList << usr;
        }
    }

    std::sort(
        mUserList.begin(), mUserList.end(), [=]( const User &u1, const User &u2 ) {
            return ( u1.uid < u2.uid );
        }
    );
};

/* List the Wayland/X11 Sessions */
void QtGreet::getLoginSessions() {

    mSessions.clear();

    QDir wlSessDir( "/usr/share/wayland-sessions" );
    for( QString sess: wlSessDir.entryList( { "*.desktop" } ) ) {
        QSettings session( wlSessDir.filePath( sess ), QSettings::IniFormat );
        if ( IsExec( session.value( "Desktop Entry/TryExec" ).toString() ) ) {
            Session s = Session{
                session.value( "Desktop Entry/Name" ).toString(),
                session.value( "Desktop Entry/Icon" ).toString(),
                "wayland",
                session.value( "Desktop Entry/Exec" ).toString(),
                wlSessDir.filePath( sess )
            };

            if ( not s.name.contains( "wayland", Qt::CaseInsensitive ) )
                s.name += " (Wayland)";

            mSessions << s;
        }
    }

    QDir xSessDir( "/usr/share/xsessions" );
    for( QString sess: xSessDir.entryList( { "*.desktop" } ) ) {
        QSettings session( xSessDir.filePath( sess ), QSettings::IniFormat );
        if ( IsExec( session.value( "Desktop Entry/TryExec" ).toString() ) ) {
            Session s = Session{
                session.value( "Desktop Entry/Name" ).toString() + " (X11)",
                session.value( "Desktop Entry/Icon" ).toString(),
                "X11",
                session.value( "Desktop Entry/Exec" ).toString(),
                xSessDir.filePath( sess )
            };

            mSessions << s;
        }
    }

    Session custom{ "Custom", "application-x-executable", "unknown", "", "" };
    mSessions << custom;
};

void QtGreet::updateUser() {

    QString username = QString( "%1 (%2)" ).arg( mUserList.at( curUser ).name ).arg( mUserList.at( curUser ).username );
    userName->setText( username );

    QString last = userConfig->value( "LastUsed/" + QString::number( mUserList.at( curUser ).uid ) ).toString();
    /* Not saved */
    if ( last.isEmpty() ) {

        return;
    }

    /* Custom Session */
    else if ( not last.startsWith( "/" ) ) {
        curSess = mSessions.count() - 1;
        updateSess();
        QString cmd = last.split( ":" ).value( 1 );
        sessionCmd->setText( cmd );
    }

    /* System Session */
    else {
        for( int i = 0; i < mSessions.count();  i++ ) {
            if ( last == mSessions.at( i ).file ) {
                curSess = i;
                updateSess();
                break;
            }
        }
    }
};

void QtGreet::updateSess() {

    session->setIcon( QIcon::fromTheme( mSessions.at( curSess ).icon, QIcon( ":/icons/session.png" ) ) );

    session->setText( mSessions.at( curSess ).name );
    sessionCmd->setText( mSessions.at( curSess ).exec );

    if ( mSessions.at( curSess ).name == "Custom" )
        sessionCmd->setReadOnly( false );

    else
        sessionCmd->setReadOnly( true );
};

void QtGreet::paintEvent( QPaintEvent *pEvent ) {

    QPainter painter( this );

    /* Base color */
    painter.save();
    painter.setPen( Qt::NoPen );
    painter.setBrush( baseColor );
    painter.drawRect( rect() );
    painter.restore();

    if ( drawBG ) {
        /* Background Image */
        painter.save();
        painter.setOpacity( 0.5 );
        painter.drawImage( QPointF( 0, 0 ), background );
        painter.setOpacity( 1.0 );
        painter.restore();
    }

    painter.save();
    painter.setPen( Qt::NoPen );
    painter.setBrush( QColor( 0, 0, 0, 180 ) );
    painter.drawRect( rect() );
    painter.restore();

    painter.end();

    QMainWindow::paintEvent( pEvent );
};

void QtGreet::timerEvent( QTimerEvent *tEvent ) {

    QMainWindow::timerEvent( tEvent );
};

void QtGreet::nextUser() {

    /* If the current user is not the last one */
    if ( curUser < mUserList.count() - 1 )
        curUser++;

    else
        curUser = 0;

    updateUser();
};

void QtGreet::previousUser() {

    /* If the current user is not the first one */
    if ( curUser > 0 )
        curUser--;

    else
        curUser = mUserList.count() - 1;

    updateUser();
};

void QtGreet::nextSess() {

    /* If the current user is not the last one */
    if ( curSess < mSessions.count() - 1 )
        curSess++;

    else
        curSess = 0;

    updateSess();
};

void QtGreet::previousSess() {

    /* If the current user is not the first one */
    if ( curSess > 0 )
        curSess--;

    else
        curSess = mSessions.count() - 1;

    updateSess();
};

void QtGreet::showUserSelection() {

    userLW->clear();

    userLW->show();
    sessionLW->hide();

    for( int i = 0; i < mUserList.count(); i++ ) {
        User user = mUserList.at( i );
        QListWidgetItem *item = new QListWidgetItem( QIcon( ":/icons/user.png" ), user.name + " (" + user.username + ")" );
        item->setData( Qt::UserRole + 1, i );

        userLW->addItem( item );
    }

    overLay->show();
};

void QtGreet::showSessionSelection() {

    sessionLW->clear();

    userLW->hide();
    sessionLW->show();

    for( int i = 0; i < mSessions.count(); i++ ) {
        Session sess = mSessions.at( i );
        QListWidgetItem *item = new QListWidgetItem( QIcon::fromTheme( sess.icon, QIcon( ":/icons/session.png" ) ), sess.name );
        item->setData( Qt::UserRole + 1, i );

        sessionLW->addItem( item );
    }

    overLay->show();
};

void QtGreet::showValidating() {

    QWidget *base = new QWidget( this );
    base->setObjectName( "OverLay" );
    base->setFixedSize( size() );

    QWidget *msg = new QWidget();
    msg->setObjectName( "widget" );

    validating->setText( "<b>Validating...</b>" );

    QPushButton *btn = new QPushButton( QIcon( ":/icons/arrow-left.png" ), "Back" );
    btn->setFixedSize( 100, 36 );
    connect( btn, &QPushButton::clicked, base, &QWidget::close );

    QGridLayout *lyt = new QGridLayout();
    lyt->setContentsMargins( QMargins( 100, 100, 100, 100 ) );
    lyt->setSpacing( 50 );
    lyt->addWidget( validating, 0, 0, Qt::AlignCenter );
    lyt->addWidget( btn, 1, 0, Qt::AlignCenter );

    msg->setLayout( lyt );

    QGridLayout *blyt = new QGridLayout();
    blyt->addWidget( msg, 0, 0, Qt::AlignCenter );

    base->setLayout( blyt );

    base->show();
};

QString QtGreet::getXSessionCommand() {

    QString xinit( "xinit %1 -- /etc/X11/xinit/xserverrc :%2 vt%3 -keeptty -noreset -novtswitch -auth /tmp/Xauth.%4" );

    /* Arg1: Session command */
    QString sessCmd = mSessions.at( curSess ).exec;

    /* Arg2: Get the display */
    int display;
    for( display = 0; display < 64; display++ ) {
        QString x1 = QString( "/tmp/.X%1-lock" ).arg( display );
        QString x2 = QString( "/tmp/.X11-unix/X%1" ).arg( display );

        if ( QFile::exists( x1 ) or QFile::exists( x2 ) )
            continue;

        else
            break;
    }

    /* Arg3: Get the vt from config.toml */
    QSettings toml( "/etc/greetd/config.toml", QSettings::IniFormat );
    int vt = toml.value( "terminal/vt" ).toInt();

    /* Arg4: Random strings for server auth file */
    QString hash = QCryptographicHash::hash( QDateTime::currentDateTime().toString().toUtf8(), QCryptographicHash::Md5 ).toHex().left( 10 );

    return xinit.arg( sessCmd ).arg( display ).arg( vt ).arg( hash );
};

void QtGreet::tryLogin() {

    showValidating();
    setDisabled( true );

    /* Save this as last used */
    QString user = QString( "LastUsed/%1" ).arg( mUserList.at( curUser ).uid );
    QString last = ( mSessions.at( curSess ).type == "unknown" ? "Custom:" + sessionCmd->text() : mSessions.at( curSess ).file );
    userConfig->setValue( user, last );
    userConfig->sync();

    qApp->processEvents();

    /* Open the request to create a session */
    request req_create_sess = {
        .request_type = request_type_create_session,
    };

    /* Add the username  */
    QString un( mUserList.at( curUser ).username );
    strncpy( req_create_sess.body.request_create_session.username, un.toUtf8().constData(), 127 );

    /* Send this request */
    response resp = roundtrip( req_create_sess );

    /* The response ideally can only be for authentication */
    /* Anything else will be considered as auth error */
    if ( resp.response_type == response_type_auth_message ) {
        request req = {
            .request_type = request_type_post_auth_message_response,
        };

        strncpy( req.body.request_post_auth_message_response.response, userPass->text().toUtf8().constData(), 127 );
        resp = roundtrip( req );

        /* Authentication should be successful; anything else will be considered a failure */
        if ( resp.response_type == response_type_success ) {
            request req = {
                .request_type = request_type_start_session,
            };

            QString cmd;
            QString logName = "/var/log/QtGreet-" + QDateTime::currentDateTime().toString( "ddMMyyyy-hhmmss" );
            if ( mSessions.at( curSess ).type == "wayland" )
                cmd = mSessions.at( curSess ).exec + " > " + logName + " 2>&1";

            else if ( mSessions.at( curSess ).type == "X11" )
                cmd = getXSessionCommand() + " > " + logName + " 2>&1";

            else
                cmd = sessionCmd->text();

            strncpy( req.body.request_start_session.cmd, cmd.toUtf8().constData(), 256 );
            resp = roundtrip( req );

            if ( resp.response_type == response_type_success )
                qApp->quit();

            else
                goto failure;
        }

        else
            goto failure;
    }

    else
        goto failure;

    failure: {
        qDebug() << "Error:" << resp.body.response_error.description;
        // Show validation dialog - failure
        validating->setText( "<b><font color='red'>Authentication failure..!!</font></b>" );
        request req_cancel_sess = {
            .request_type = request_type_cancel_session,
        };
        roundtrip( req_cancel_sess );

        /* Enable the dialog */
        setEnabled( true );

        // TODO: RESET THE LOGIN DIALOG
    }
};

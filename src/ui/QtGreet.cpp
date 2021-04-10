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

    QFile qss( "/home/cosmos/Softwares/Projects/QtGreet/themes/sidebar/style.qss" );
    qss.open( QFile::ReadOnly );
    QString ss = QString::fromLocal8Bit( qss.readAll() );
    qss.close();

    return ss.replace( "<text_color>", textColor );
};

QtGreet::QtGreet() {

	setFixedSize( qApp->primaryScreen()->size() );
    createUI();

	setStyleSheet( getStyleSheet( "#ffffff" ) );

	QString bgStr( "/home/cosmos/Softwares/Projects/QtGreet/backgrounds/Sailboats.svg" );
    background = QImage( bgStr ).scaled( size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );
};

void QtGreet::createUI() {

    LayoutManager lytMgr( "/home/cosmos/Softwares/Projects/QtGreet/themes/sidebar/layout.hjson" );

    QWidget *w = new QWidget();
    lytMgr.applyLayout( w );

    setCentralWidget( w );
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

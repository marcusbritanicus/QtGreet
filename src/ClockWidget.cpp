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
#include "ClockWidget.hpp"

ClockWidget::ClockWidget( QColor txtclr, QWidget *parent ) : QWidget( parent ) {

	mTextColor = txtclr;
	mShadowColor = QColor( 255 - txtclr.red(), 255 - txtclr.green(), 255 - txtclr.blue() );

	QTimer *timer = new QTimer( this );
	connect( timer, SIGNAL( timeout() ), this, SLOT( update() ) );
	timer->start( 250 );

	setWindowTitle( tr( "Analog Clock" ) );
	resize( 200, 200 );
};

void ClockWidget::paintEvent( QPaintEvent * ) {

	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing );

	/* Size of the square */
	int side = qMin( width() - 20, height() - 20 );

	/* Draw battery circle track */
	painter.save();;
	painter.setPen( QPen( mTextColor, ( side > 100 ? 6 : ( side > 50 ? 4 : 2 ) ), Qt::SolidLine, Qt::RoundCap ) );
	painter.drawArc( QRect( (width() - side) / 2, (height() - side) / 2, side, side ), 90.0 * 16.0, -360.0 * 16.0 );
	painter.restore();

	/* Get the current time */
	QDateTime datetime = QDateTime::currentDateTime();

	QDate date = datetime.date();
	QTime time = datetime.time();

	/* Clock center dot */
	painter.save();
	painter.setPen( Qt::NoPen );
	painter.setBrush( mTextColor );
	painter.drawEllipse( QPointF( width() / 2, height() / 2 ), 3, 3 );
	painter.restore();

	painter.translate( width() / 2, height() / 2 );
	painter.scale( ( side - 10 ) / 200.0, ( side - 10 ) / 200.0 );

	/* Hour/minutes tics */
	painter.setPen( QPen( mTextColor, 2.0, Qt::SolidLine, Qt::RoundCap ) );
	for ( int j = 0; j < 60; ++j ) {
		if ( ( j % 5 ) == 0 )
			painter.drawLine( 88, 0, 96, 0 );

		else
			painter.drawLine( 92, 0, 96, 0 );

		painter.rotate( 6.0 );
	}

	/* Hour Hand */
	painter.save();
	painter.rotate( 30.0 * ( ( time.hour() + time.minute() / 60.0 ) ) );
	painter.setPen( QPen( mTextColor, 5.0, Qt::SolidLine, Qt::RoundCap ) );
	painter.drawLine( QLineF( QPointF( 0, -8 ), QPointF( 0, -40 ) ) );
	painter.restore();

	/* Minute Hand */
	painter.save();
	painter.rotate( 6.0 * ( time.minute() + time.second() / 60.0 ) );
	painter.setPen( QPen( mTextColor, 4.0, Qt::SolidLine, Qt::RoundCap ) );
	painter.drawLine( QLineF( QPointF( 0, -8 ), QPointF( 0, -70 ) ) );
	painter.restore();

	/* Second Hand */
	painter.save();
	painter.rotate( 6.0 * ( time.second() + time.msec() / 1000.0 ) );
	painter.setPen( QPen( Qt::red, 2.0, Qt::SolidLine, Qt::RoundCap ) );
	painter.drawLine( QLineF( QPointF( 0, -8 ), QPointF( 0, -80 ) ) );
	painter.restore();

	/* Date */
	painter.resetTransform();
	painter.save();
	QFont f( font() );
	painter.setFont( QFont( f.family(), f.pointSize() + 1, QFont::Bold ) );
	painter.setPen( mShadowColor );
	painter.drawText( QRectF( 1, height() * 0.70 + 1, width(), height() ), Qt::AlignHCenter | Qt::AlignTop, date.toString( "MMM dd, yyyy" ) );
	painter.setPen( mTextColor );
	painter.drawText( QRectF( 0, height() * 0.70 + 0, width(), height() ), Qt::AlignHCenter | Qt::AlignTop, date.toString( "MMM dd, yyyy" ) );
	painter.restore();

	/* End the painting */
	painter.end();
};

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
#include <QtDBus>

#include "ClockWidget.hpp"

/*
	*
	* Analog Clock
	*
*/

AnalogClock::AnalogClock( QColor txtclr, QWidget *parent ) : QWidget( parent ) {

	mTextColor = txtclr;
	mShadowColor = QColor( 255 - txtclr.red(), 255 - txtclr.green(), 255 - txtclr.blue() );

	QTimer *timer = new QTimer( this );
	connect( timer, SIGNAL( timeout() ), this, SLOT( update() ) );
	timer->start( 250 );

	resize( 200, 200 );

	QImage bImg = QImage( ":/icons/battery.png" ).scaled( QSize( 24, 24 ), Qt::KeepAspectRatio, Qt::SmoothTransformation );
	QImage aImg = QImage( ":/icons/acpower.png" ).scaled( QSize( 24, 24 ), Qt::KeepAspectRatio, Qt::SmoothTransformation );

	battery = new BatteryInfo();
	mCharge = battery->currentCharge();
	connect(
		battery, &BatteryInfo::chargeChanged, [=]( qreal bCharge ) {
			mCharge = bCharge;
			repaint();
		}
	);

	mOnBattery = battery->onBattery();
	connect(
		battery, &BatteryInfo::onBatteryChanged, [=]( bool yes ) {
			mOnBattery = yes;
			if ( mOnBattery )
				battImg = bImg;

			else
				battImg = aImg;

			repaint();
		}
	);

	if ( mOnBattery )
		battImg = bImg;

	else
		battImg = aImg;
};

void AnalogClock::paintEvent( QPaintEvent * ) {

	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing );

	/* Size of the square */
	int side = qMin( width() - 20, height() - 20 );

	/* Draw battery circle track */
	painter.save();
	painter.setPen( QPen( mTextColor, ( side > 100 ? 6 : ( side > 50 ? 4 : 2 ) ), Qt::SolidLine, Qt::RoundCap ) );
	painter.drawArc( QRect( (width() - side) / 2, (height() - side) / 2, side, side ), 90.0 * 16.0, -360.0 * 16.0 * mCharge / 100.0 );
	painter.restore();

	/* Battery/AC info: 24px */
	painter.save();
	painter.drawImage( QRect( ( width() - 24 ) / 2.0, 30, 24, 24 ), battImg );
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

/*
	*
	* Digital Clock
	*
*/

DigitalClock::DigitalClock( QWidget *parent ) : QWidget( parent ) {

	QDateTime dt = QDateTime::currentDateTime();
	int size = font().pointSize() * 16.0 / 12.0 * 2.0;

	time = new QLCDNumber( this );
	time->setDigitCount( 5 );
	time->setSegmentStyle( QLCDNumber::Flat );
	time->setFrameStyle( QLCDNumber::NoFrame );
	time->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	time->setMinimumHeight( size * 2 );
	time->display( dt.toString( "hh:mm" ) );

	secs = new QLCDNumber( this );
	secs->setDigitCount( 2 );
	secs->setSegmentStyle( QLCDNumber::Flat );
	secs->setFrameStyle( QLCDNumber::NoFrame );
	secs->setFixedSize( QSize( size, size ) );
	secs->display( dt.toString( "ss" ) );

	ampm = new QLabel();
	ampm->setAlignment( Qt::AlignCenter );
	ampm->setFixedSize( QSize( size, size ) );
	ampm->setFixedSize( QSize( size, size ) );
	ampm->setText( dt.time().hour() > 12 ? "PM" : "AM" );

	date = new QLabel();
	date->setAlignment( Qt::AlignCenter );
	date->setText( dt.toString( "ddd, MMM dd, yyyy" ) );

	alrm = new QLabel();
	alrm->setAlignment( Qt::AlignCenter );
	alrm->setText( QString::fromUtf8( "\xF0\x9F\x94\x94" ) );

	QGridLayout *lyt = new QGridLayout();
	lyt->addWidget( time, 0, 0, 2, 1 );
	lyt->addWidget( secs, 0, 1, Qt::AlignCenter );
	lyt->addWidget( ampm, 1, 1, Qt::AlignCenter );
	lyt->addWidget( date, 2, 0, Qt::AlignCenter );
	lyt->addWidget( alrm, 2, 1, Qt::AlignCenter );
	setLayout( lyt );

	setFixedHeight( size * 3.5 );

	QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
	shadow->setOffset( 1, 1 );
	shadow->setColor( palette().color( QPalette::Window ) );
	setGraphicsEffect( shadow );

	timer = new QBasicTimer();
	timer->start( 1000, this );
};

void DigitalClock::timerEvent( QTimerEvent *event ) {

	if ( event->timerId() == timer->timerId() ) {

		QDateTime dt = QDateTime::currentDateTime();
		time->display( dt.toString( "hh:mm" ) );
		secs->display( dt.toString( "ss" ) );
		ampm->setText( dt.time().hour() > 12 ? "PM" : "AM" );
		date->setText( dt.toString( "ddd, MMM dd, yyyy" ) );
	}

	QWidget::timerEvent( event );
};

/*
	*
	* Simple Clock
	*
*/

SimpleClock::SimpleClock( bool time, bool date, QWidget *parent ) {

	timeFmt = "HH:mm";
	dateFmt = "dddd, MMMM dd, yyyy";

	// Text inside the QLabel
	setAlignment( Qt::AlignCenter );

	if ( not date and not time ) {
		date = true;
		time = true;
	}

	mShowTime = time;
	mShowDate = date;

	QDateTime dt = QDateTime::currentDateTime();
	if ( time and date ) {
		clockFmt = "<center><large>%1</large><br>%2</center>";
		setText( QString( clockFmt ).arg( dt.toString( timeFmt ) ).arg( dt.toString( dateFmt ) ) );
	}

	else {
		clockFmt = "<center>%1</center>";
		setText( QString( clockFmt ).arg( dt.toString( time ? timeFmt : dateFmt ) ) );
	}

	QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
	shadow->setOffset( 1, 1 );
	shadow->setColor( palette().color( QPalette::Window ) );
	setGraphicsEffect( shadow );

	timer = new QBasicTimer();
	timer->start( 1000, this );
};

void SimpleClock::setDateFormat( QString newFmt ) {

	dateFmt = newFmt;
};

void SimpleClock::setTimeFormat( QString newFmt ) {

	timeFmt = newFmt;
};

void SimpleClock::timerEvent( QTimerEvent *event ) {

	if ( event->timerId() == timer->timerId() ) {

		QDateTime dt = QDateTime::currentDateTime();
		if ( mShowTime and mShowDate )
			setText( QString( clockFmt ).arg( dt.toString( timeFmt ) ).arg( dt.toString( dateFmt ) ) );

		else
			setText( QString( clockFmt ).arg( dt.toString( mShowTime ? timeFmt : dateFmt ) ) );
	}

	QLabel::timerEvent( event );
};

/*
	*
	* Battery Info
	*
*/

BatteryInfo::BatteryInfo() : QObject() {

	iface = new QDBusInterface(
        "org.freedesktop.UPower",
        "/org/freedesktop/UPower/devices/DisplayDevice",
        "org.freedesktop.UPower.Device",
        QDBusConnection::systemBus()
    );

	QDBusConnection::systemBus().connect(
        "org.freedesktop.UPower",
        "/org/freedesktop/UPower/devices/DisplayDevice",
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT( handlePowerChanges( QString, QVariantMap, QStringList ) )
    );

	switch( iface->property( "State" ).toUInt() ) {
		case 1:
		case 4: {
			mOnBattery = false;
			break;
		}

		default: {
			mOnBattery = true;
			break;
		}
	};

	mLastPercentage = iface->property( "Percentage" ).toDouble();
};

qreal BatteryInfo::currentCharge() {

	return mLastPercentage;
};

bool BatteryInfo::onBattery() {

	return mOnBattery;
};

void BatteryInfo::handlePowerChanges( QString interface, QVariantMap valueMap, QStringList ) {

	bool battery;
	switch( iface->property( "State" ).toUInt() ) {
		case 1:
		case 4: {
			battery = false;
			break;
		}

		default: {
			battery = true;
			break;
		}
	};

    /* Check for Power State Changes */
    if ( battery != mOnBattery ) {
        mOnBattery = battery;
        if ( mOnBattery )
            emit onBatteryChanged( true );

        else
            emit onBatteryChanged( false );
    }

    /* Battery percentage */
    double curBatPC = iface->property( "Percentage" ).toDouble();
    if ( mLastPercentage != curBatPC ) {
        mLastPercentage = curBatPC;
        emit chargeChanged( mLastPercentage );
    }
};

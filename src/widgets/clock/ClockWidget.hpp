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

#pragma once

#include <QWidget>
#include <QVariantMap>

class QDBusInterface;
class BatteryInfo;

class AnalogClock : public QWidget {
	Q_OBJECT;

	public:
		AnalogClock( QColor text, QWidget *parent = 0 );

	private:
		QColor mTextColor;
		QColor mShadowColor;

		BatteryInfo *battery;
		qreal mCharge = 100.0;
		bool mOnBattery = false;
		QImage battImg;

	protected:
		void paintEvent( QPaintEvent *event ) override;
};

class DigitalClock : public QWidget {
	Q_OBJECT

	public:
		DigitalClock(QWidget *parent = 0);

	private:
		QLCDNumber *time;
		QLCDNumber *secs;
		QLabel     *ampm;
		QLabel     *date;
		QLabel	   *alrm;

		QBasicTimer *timer;

	protected:
		void timerEvent( QTimerEvent *event ) override;
};

class SimpleClock : public QLabel {
	Q_OBJECT

	public:
		SimpleClock( bool time, bool date, QWidget *parent = 0);

		void setDateFormat( QString );
		void setTimeFormat( QString );

	private:
		QBasicTimer *timer;
		QString timeFmt;
		QString dateFmt;
		QString clockFmt;

		bool mShowTime = true;
		bool mShowDate = true;

	protected:
		void timerEvent( QTimerEvent *event ) override;
};

class BatteryInfo : public QObject {
	Q_OBJECT;

	public:
		BatteryInfo();

		qreal currentCharge();
		bool onBattery();

	private:
		QDBusInterface *iface;
		qreal mLastPercentage = 100.0;
		bool mOnBattery = false;

	private Q_SLOTS:
		void handlePowerChanges( QString, QVariantMap, QStringList );

	Q_SIGNALS:
		/* Battery Charge/Discharge Signals */
		void onBatteryChanged( bool );

		/* Battery charge changed */
		void chargeChanged( double );
};

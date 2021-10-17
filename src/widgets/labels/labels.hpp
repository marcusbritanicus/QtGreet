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

#include <QtWidgets>

class LockState: public QWidget {
    Q_OBJECT;

    public:
        LockState( QString type );

        void setState( bool state );
        void toggle();

    private:
        bool isOn = false;
        QFont mFont;
        QColor textClr;
        QString text;

    protected:
        void paintEvent( QPaintEvent *pEvent );
};

class UserIcon: public QLabel {
    Q_OBJECT;

    public:
        UserIcon();

        void setPixmap( QString );

    private:
        QImage img;
        QSize iconSize;

    protected:
        void paintEvent( QPaintEvent * );
};

class Logo: public QWidget {
    Q_OBJECT;

    public:
        Logo();

        void setIconSize( QSize );
        void setIcon( QString );

    private:
        int mIconSize;
        QString mIcon;
        QPixmap mPix;

    protected:
        void paintEvent( QPaintEvent *pEvent );
};

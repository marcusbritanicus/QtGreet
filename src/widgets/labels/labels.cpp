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

#include <QtWidgets>

#include "labels.hpp"

LockState::LockState( QString type ) : QWidget() {
    /* Initial Status */
    QFile file;

    if ( type == "CapsLock" ) {
        setObjectName( "CapsLock" );
        file.setFileName( "/sys/class/leds/input0::capslock/brightness" );
        text = "A";
    }

    else {
        setObjectName( "NumLock" );
        file.setFileName( "/sys/class/leds/input0::numlock/brightness" );
        text = "#";
    }

    if ( file.open( QFile::ReadOnly ) ) {
        if ( file.readAll().toInt() ) {
            isOn = true;
        }

        else {
            isOn = false;
        }

        file.close();
    }

    mFont = font();
    mFont.setPointSize( qMin( width(), height() ) - 4 );
    mFont.setWeight( QFont::Bold );

    setFont( mFont );

    textClr = palette().color( QPalette::Text );

    setMinimumSize( QSize( 16, 16 ) );
}


void LockState::setState( bool state ) {
    isOn = state;
    repaint();
}


void LockState::toggle() {
    isOn = not isOn;
    repaint();
}


void LockState::paintEvent( QPaintEvent *pEvent ) {
    QPainter painter( this );

    painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

    if ( isOn ) {
        textClr.setAlphaF( 1.0 );
    }

    else {
        textClr.setAlphaF( 0.2 );
    }

    painter.setPen( textClr );
    painter.drawText( rect(), Qt::AlignCenter, text );

    painter.end();

    pEvent->accept();
}


UserIcon::UserIcon() : QLabel() {
    setObjectName( "UserIcon" );
    setAlignment( Qt::AlignCenter );
    setFixedSize( QSize( 72, 72 ) );

    iconSize = QSize( 64, 64 );

    img = QImage( ":/icons/user.png" );
}


void UserIcon::setPixmap( QString path ) {
    img = QImage( path );
    update();
}


void UserIcon::paintEvent( QPaintEvent *pEvent ) {
    QPainter painter( this );

    painter.setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform );

    QImage brush = img.scaled( iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );

    if ( brush.isNull() ) {
        brush = QImage( ":/icons/user.png" ).scaled( iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    }

    painter.save();
    painter.setPen( QPen( Qt::gray, 2.0 ) );
    painter.drawRoundedRect( rect().adjusted( 1, 1, -1, -1 ), 5.0, 5.0 );
    painter.restore();

    painter.save();
    painter.translate( 4, 4 );
    painter.setBrush( brush );
    painter.setPen( Qt::NoPen );
    painter.drawRoundedRect( QRectF( QPointF( 0, 0 ), iconSize ), 5.0, 5.0 );
    painter.restore();

    painter.end();

    pEvent->accept();
}


Logo::Logo() : QWidget() {
    setObjectName( "Logo" );
    setFixedSize( 40, 40 );
    mIcon = ":/icons/QtGreet.png";
}


void Logo::setIconSize( QSize size ) {
    mIconSize = size.width();
    mPix      = QPixmap( mIcon ).scaled( size, Qt::KeepAspectRatio, Qt::SmoothTransformation );

    repaint();
}


void Logo::setIcon( QString img ) {
    mIcon = img;
    QSize size( mIconSize, mIconSize );

    mPix = QPixmap( mIcon ).scaled( size, Qt::KeepAspectRatio, Qt::SmoothTransformation );

    repaint();
}


void Logo::paintEvent( QPaintEvent *pEvent ) {
    QPainter painter( this );

    painter.setRenderHints( QPainter::Antialiasing );
    painter.drawPixmap( QRect( (width() - mIconSize) / 2, (height() - mIconSize) / 2, mIconSize, mIconSize ), mPix );
    painter.end();

    pEvent->accept();
}

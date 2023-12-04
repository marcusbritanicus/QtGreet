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

#include <iostream>

#include "Container.hpp"

Container::Container( Hjson::Value bgClr, QString name ) : QWidget() {
    /** Object Name */
    setObjectName( name );

    /** Default Background will be transparent */
    bgColor = QColor( Qt::transparent );

    if ( not bgClr.empty() ) {
        int red, green, blue, alpha;
        red   = bgClr[ "Red" ].to_double() * 255;
        green = bgClr[ "Green" ].to_double() * 255;
        blue  = bgClr[ "Blue" ].to_double() * 255;
        alpha = bgClr[ "Alpha" ].to_double() * 255;

        bgColor = QColor( red, green, blue, alpha );
    }
}


void Container::paintEvent( QPaintEvent *pEvent ) {
    if ( bgColor != QColor( Qt::transparent ) ) {
        QPainter painter( this );

        painter.setBrush( bgColor );
        painter.setPen( Qt::NoPen );

        painter.drawRoundedRect( rect(), 5, 5 );

        painter.end();
    }

    QWidget::paintEvent( pEvent );
}

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

#include <iostream>

#include "LayoutUtils.hpp"
#include "WidgetFactory.hpp"

bool isNumeric( Hjson::Value val ) {

    switch( val.type() ) {
        case Hjson::Type::Double:
        case Hjson::Type::Int64:
            return true;

        default:
            return false;
    }

    return false;
};

QMargins getMargins( Hjson::Value margins ) {

    if ( margins.empty() or not margins.defined() )
        return QMargins();

    double left = 0;
    double right = 0;
    double top = 0;
    double bottom = 0;

    if ( isNumeric( margins ) )
        left = right = top = bottom = margins.to_double();

    else if ( margins.type() == Hjson::Type::Vector ) {
        if ( margins.size() < 4 )
            left = right = top = bottom = margins.to_double();

        else {
            left   = margins[ 0 ];
            right  = margins[ 1 ];
            top    = margins[ 2 ];
            bottom = margins[ 3 ];
        }
    }

    else {
        if ( isNumeric( margins[ "Left" ] ) )
            left = margins[ "Left" ];

        if ( isNumeric( margins[ "Right" ] ) )
            right = margins[ "Right" ];

        if ( isNumeric( margins[ "Top" ] ) )
            top = margins[ "Top" ];

        if ( isNumeric( margins[ "Bottom" ] ) )
            bottom = margins[ "Bottom" ];
    }

    if ( left < 1.0 )
        left *= mScreenSize.width();

    if ( right < 1.0 )
        right *= mScreenSize.width();

    if ( top < 1.0 )
        top *= mScreenSize.height();

    if ( bottom < 1.0 )
        bottom *= mScreenSize.height();

    return QMargins( left, top, right, bottom );
};

int getSpacing( Hjson::Value value ) {

    if ( value.empty() or not value.defined() )
        return 0;

    if ( value.type() != Hjson::Type::Int64 )
        return 0;

    return value.to_int64();
};

Qt::Alignment getAlignment( Hjson::Value value ) {

    QString align( value.to_string().c_str() );
    Qt::Alignment XX;

    if ( align[ 0 ] == "T" )
        XX = Qt::AlignTop;

    else if ( align[ 0 ] == "M" )
        XX = Qt::AlignVCenter;

    else if ( align[ 0 ] == "B" )
        XX = Qt::AlignBottom;

    if ( align[ 1 ] == "L" )
        XX |= Qt::AlignLeft;

    else if ( align[ 1 ] == "M" )
        XX |= Qt::AlignHCenter;

    else if ( align[ 1 ] == "R" )
        XX |= Qt::AlignRight;

    return XX;
};

int getWidth( Hjson::Value obj ) {

    double width = obj.to_double();
    if ( width <= 1.0 )
        width *= mScreenSize.width();

    return (int)width;
};

int getHeight( Hjson::Value obj ) {

    double height = obj.to_double();
    if ( height <= 1.0 )
        height *= mScreenSize.height();

    return (int)height;
};

QString getType( Hjson::Value type ) {

    return type.to_string().c_str();
};

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

#include "LayoutUtils.hpp"
#include "WidgetFactory.hpp"

bool isNumeric( Hjson::Value val ) {
    switch ( val.type() ) {
        case Hjson::Type::Double:
        case Hjson::Type::Int64: {
            return true;
        }

        default: {
            return false;
        }
    }

    return false;
}


QMargins getMargins( Hjson::Value margins, QSize screenSize ) {
    if ( margins.empty() or not margins.defined() ) {
        return QMargins();
    }

    double left   = 0;
    double right  = 0;
    double top    = 0;
    double bottom = 0;

    if ( isNumeric( margins ) ) {
        left = right = top = bottom = margins.to_double();
    }

    else if ( margins.type() == Hjson::Type::Vector ) {
        if ( margins.size() < 4 ) {
            left = right = top = bottom = margins.to_double();
        }

        else {
            left   = margins[ 0 ];
            right  = margins[ 1 ];
            top    = margins[ 2 ];
            bottom = margins[ 3 ];
        }
    }

    else {
        if ( isNumeric( margins[ "Left" ] ) ) {
            left = margins[ "Left" ];
        }

        if ( isNumeric( margins[ "Right" ] ) ) {
            right = margins[ "Right" ];
        }

        if ( isNumeric( margins[ "Top" ] ) ) {
            top = margins[ "Top" ];
        }

        if ( isNumeric( margins[ "Bottom" ] ) ) {
            bottom = margins[ "Bottom" ];
        }
    }

    if ( left < 1.0 ) {
        left *= screenSize.width();
    }

    if ( right < 1.0 ) {
        right *= screenSize.width();
    }

    if ( top < 1.0 ) {
        top *= screenSize.height();
    }

    if ( bottom < 1.0 ) {
        bottom *= screenSize.height();
    }

    return QMargins( left, top, right, bottom );
}


int getSpacing( Hjson::Value value, QSize ) {
    if ( value.empty() or not value.defined() ) {
        return 0;
    }

    if ( value.type() != Hjson::Type::Int64 ) {
        return 0;
    }

    return value.to_int64();
}


Qt::Alignment getAlignment( Hjson::Value value ) {
    QString       align( value.to_string().c_str() );
    Qt::Alignment XX;

    /** We'll manage the alignment externally */
    if ( align.isEmpty() ) {
        return XX;
    }

    if ( align.at( 0 ) == QChar( 'T' ) ) {
        XX = Qt::AlignTop;
    }

    else if ( align.at( 0 ) == QChar( 'M' ) ) {
        XX = Qt::AlignVCenter;
    }

    else if ( align.at( 0 ) == QChar( 'B' ) ) {
        XX = Qt::AlignBottom;
    }

    if ( align.size() == 2 ) {
        if ( align.at( 1 ) == QChar( 'L' ) ) {
            XX |= Qt::AlignLeft;
        }

        else if ( align.at( 1 ) == QChar( 'M' ) ) {
            XX |= Qt::AlignHCenter;
        }

        else if ( align.at( 1 ) == QChar( 'R' ) ) {
            XX |= Qt::AlignRight;
        }
    }

    else {
        XX |= Qt::AlignLeft;
    }

    return XX;
}


int getWidth( Hjson::Value obj, QSize screenSize ) {
    double width = obj.to_double();

    if ( width <= 1.0 ) {
        width *= screenSize.width();
    }

    if ( width < 0 ) {
        width = 0;
    }

    return (int)width;
}


int getHeight( Hjson::Value obj, QSize screenSize ) {
    double height = obj.to_double();

    if ( height <= 1.0 ) {
        height *= screenSize.height();
    }

    if ( height < 0 ) {
        height = 0;
    }

    return (int)height;
}


QString getType( Hjson::Value type ) {
    return type.to_string().c_str();
}


QFont::Weight getFontWeight( Hjson::Value weight ) {
    if ( weight.type() == Hjson::Type::String ) {
        std::string w( weight );

        if ( w == "Thin" ) {
            return QFont::Thin;
        }

        else if ( w == "ExtraLight" ) {
            return QFont::ExtraLight;
        }

        else if ( w == "Light" ) {
            return QFont::Light;
        }

        else if ( w == "Normal" ) {
            return QFont::Normal;
        }

        else if ( w == "Medium" ) {
            return QFont::Medium;
        }

        else if ( w == "DemiBold" ) {
            return QFont::DemiBold;
        }

        else if ( w == "Bold" ) {
            return QFont::Bold;
        }

        else if ( w == "ExtraBold" ) {
            return QFont::ExtraBold;
        }

        else if ( w == "Black" ) {
            return QFont::Black;
        }

        else {
            return QFont::Normal;
        }
    }

    else if ( weight.type() == Hjson::Type::Int64 ) {
        int w( weight );

        if ( w >= 87 ) {
            return QFont::Black;
        }

        else if ( w >= 81 ) {
            return QFont::ExtraBold;
        }

        else if ( w >= 75 ) {
            return QFont::Bold;
        }

        else if ( w >= 63 ) {
            return QFont::DemiBold;
        }

        else if ( w >= 57 ) {
            return QFont::Medium;
        }

        else if ( w >= 50 ) {
            return QFont::Normal;
        }

        else if ( w >= 25 ) {
            return QFont::Light;
        }

        else if ( w >= 12 ) {
            return QFont::ExtraLight;
        }

        else {
            return QFont::Thin;
        }
    }

    return QFont::Normal;
}

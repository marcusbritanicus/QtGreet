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

#include "LayoutManager.hpp"
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

    QSize screenSize = qApp->primaryScreen()->size();

    double left = 0;
    double right = 0;
    double top = 0;
    double bottom = 0;

    if ( isNumeric( margins[ "Left" ] ) )
        left = ( margins[ "Left" ] < 1.0 ? screenSize.width() * margins[ "Left" ] : margins[ "Left" ] );

    if ( isNumeric( margins[ "Right" ] ) )
        right = ( margins[ "Right" ] < 1.0 ? screenSize.width() * margins[ "Right" ] : margins[ "Right" ] );

    if ( isNumeric( margins[ "Top" ] ) )
        top = ( margins[ "Top" ] < 1.0 ? screenSize.width() * margins[ "Top" ] : margins[ "Top" ] );

    if ( isNumeric( margins[ "Bottom" ] ) )
        bottom = ( margins[ "Bottom" ] < 1.0 ? screenSize.width() * margins[ "Bottom" ] : margins[ "Bottom" ] );

    return QMargins( left, top, right, bottom );
};

Qt::Alignment getAlignment( Hjson::Value obj ) {

    // @vertical: Layout direction

    if ( obj[ "Alignment" ].type() == Hjson::Type::String ) {

        // Left alignment
        if ( obj[ "Alignment" ] == "Left" )
            return Qt::AlignLeft;

        // Right alignment
        else if ( obj[ "Alignment" ] == "Right" )
            return Qt::AlignRight;

        // Top alignment
        if ( obj[ "Alignment" ] == "Top" )
            return Qt::AlignTop;

        // Bottom alignment
        else if ( obj[ "Alignment" ] == "Bottom" )
            return Qt::AlignBottom;

        // Centered
        // Fall through to end of function
    }

    return Qt::AlignCenter;
};

LayoutManager::LayoutManager( QString layoutFile ) {

    screenSize = qApp->primaryScreen()->size();
    mLayout = Hjson::UnmarshalFromFile( layoutFile.toStdString() );
};

void LayoutManager::applyLayout( QWidget *widget ) {

    if ( mLayout.key( 0 ) == "VerticalLayout" )
        widget->setLayout( getVLayout( mLayout[ "VerticalLayout" ] ) );

    else
        widget->setLayout( getHLayout( mLayout[ "HorizontalLayout" ] ) );
};

QVBoxLayout* LayoutManager::getVLayout( Hjson::Value layout ) {

    Hjson::Value margins( layout[ "Margins" ] );
    int spacing = ( isNumeric( layout[ "Spacing" ] ) ? layout[ "Spacing" ] : 0 );

    QVBoxLayout *lyt = new QVBoxLayout();
    lyt->setContentsMargins( getMargins( margins ) );
    lyt->setSpacing( spacing );

    Hjson::Value objects( layout[ "Objects" ] );
    for( size_t i = 0; i < objects.size(); i++ ) {

        /* Every object in the list @objects will be a map with a single item. */
        std::string objName = Hjson::Value( layout[ "Objects" ][ i ] ).key( 0 );

        /* Get the actual object in that map */
        Hjson::Value obj( layout[ "Objects" ][ i ][ objName ] );

        /* Object alignment: default is center */
        Qt::Alignment align = getAlignment( obj );

        /* The object is a vertical layout */
        if ( objName == "VerticalLayout" )
            lyt->addLayout( getVLayout( obj ), align );

        /* The object is a horizontal layout */
        else if ( objName == "HorizontalLayout" )
            lyt->addLayout( getHLayout( obj ), align );

        /* The object is a stretch */
        else if ( objName == "Stretch" )
            lyt->addStretch();

        /* The object is a Separator */
        else if ( objName == "Separator" )
            lyt->addWidget( WidgetFactory::createWidget( "Separator", "Vertical", QVariantMap() ), align );

        /* This object is a widget (Named or a generic QWidget) */
        /* This object is a widget (Named or a generic QWidget) */
        else {
            QWidget *widget = getWidget( objName, obj );
            lyt->addWidget( widget, align );
        }
    }

    return lyt;
};

QHBoxLayout* LayoutManager::getHLayout( Hjson::Value layout ) {

    Hjson::Value margins( layout[ "Margins" ] );
    int spacing = ( isNumeric( layout[ "Spacing" ] ) ? layout[ "Spacing" ] : 0 );

    QHBoxLayout *lyt = new QHBoxLayout();
    lyt->setContentsMargins( getMargins( margins ) );
    lyt->setSpacing( spacing );

    Hjson::Value objects( layout[ "Objects" ] );
    for( size_t i = 0; i < objects.size(); i++ ) {

        /* Every object in the list @objects will be a map with a single item. */
        std::string objName = Hjson::Value( layout[ "Objects" ][ i ] ).key( 0 );

        /* Get the actual object in that map */
        Hjson::Value obj( layout[ "Objects" ][ i ][ objName ] );

        /* Object alignment: default is center */
        Qt::Alignment align = getAlignment( obj );

        /* The object is a vertical layout */
        if ( objName == "VerticalLayout" )
            lyt->addLayout( getVLayout( obj ), align );

        /* The object is a horizontal layout */
        else if ( objName == "HorizontalLayout" )
            lyt->addLayout( getHLayout( obj ), align );

        /* The object is a stretch */
        else if ( objName == "Stretch" )
            lyt->addStretch();

        /* The object is a Separator */
        else if ( objName == "Separator" )
            lyt->addWidget( WidgetFactory::createWidget( "Separator", "Horizontal", QVariantMap() ), align );

        /* This object is a widget (Named or a generic QWidget) */
        else {
            QWidget *widget = getWidget( objName, obj );
            lyt->addWidget( widget, align );
        }
    }

    return lyt;
};

QWidget* LayoutManager::getWidget( std::string objName, Hjson::Value obj ) {

    /* Let's first get the object type */
    std::string type = "";

    /* Layout info, if any. */
    bool hasLayout = false;
    std::string lytType;

    /* Now we get its properties */
    QVariantMap properties;

    for( size_t p = 0; p < obj.size(); p++ ) {
        std::string prop( obj.key( p ) );

        if ( prop == "Type" )
            type = obj[ "Type" ].to_string();

        /* If the property is a Layout */
        else if ( prop.find( "Layout" ) != std::string::npos ) {
            hasLayout = true;
            lytType = prop;
        }

        /* If the property is width related */
        else if ( prop.find( "Width" ) != std::string::npos ) {
            int width = obj[ prop ];
            if ( obj[ prop ].type() == Hjson::Type::Double )
                width = ( obj[ prop ] * screenSize.width() );

            properties[ prop.c_str() ] = QVariant::fromValue( width );
        }

        /* If the property is height related */
        else if ( prop.find( "Height" ) != std::string::npos ) {
            int height = obj[ prop ];
            if ( obj[ prop ].type() == Hjson::Type::Double )
                height = ( obj[ prop ] * screenSize.height() );

            properties[ prop.c_str() ] = QVariant::fromValue( height );
        }

        else {
            properties[ prop.c_str() ] = obj[ prop ].to_string().c_str();
        }
    }

    /* Create this widget */
    QWidget *widget = WidgetFactory::createWidget( objName.c_str(), type.c_str(), properties );

    /* If this is a generic QWidget with a Layout, apply the layout */
    if ( objName == "QWidget" and hasLayout ) {
        if ( lytType == "VerticalLayout" )
            widget->setLayout( getVLayout( obj[ "VerticalLayout" ] ) );

        else
            widget->setLayout( getHLayout( obj[ "HorizontalLayout" ] ) );
    }

    return widget;
};

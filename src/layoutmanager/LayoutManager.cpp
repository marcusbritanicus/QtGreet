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

#include "LayoutManager.hpp"
#include "WidgetFactory.hpp"

#include "Container.hpp"
#include "LayoutUtils.hpp"

QtGreet::LayoutManager::LayoutManager() {
    mScreenSize = qApp->primaryScreen()->size();
}


QBoxLayout * QtGreet::LayoutManager::generateLayout( QString lytFile ) {
    mLytFile = lytFile;

    Hjson::Value layout = Hjson::UnmarshalFromFile( lytFile.toStdString() );

    mLayout     = layout[ "Layout" ];
    mProperties = layout[ "Properties" ];

    Hjson::Value layoutItems( mLayout[ "Keys" ] );

    /** Do we have a row? If yes, we will be expanding the size vertically */
    QRegularExpression rows = QRegularExpression( "^R[0-9]+$" );

    /** Do we have a column? If yes, we will be expanding the size horizontally */
    QRegularExpression cols = QRegularExpression( "^C[0-9]+$" );

    QBoxLayout *topLyt;

    /** Vertical Layout */
    if ( mLayout[ "Direction" ].to_string() == "Vertical" ) {
        topLyt = new QBoxLayout( QBoxLayout::TopToBottom );
    }

    else {
        topLyt = new QBoxLayout( QBoxLayout::LeftToRight );
    }

    topLyt->setContentsMargins( getMargins( mLayout[ "Margins" ] ) );
    topLyt->setSpacing( getSpacing( mLayout[ "Spacing" ] ) );

    topLyt->setGeometry( QRect( QPoint( 0, 0 ), mScreenSize ) );

    for ( int idx = 0; idx < (int)layoutItems.size(); ++idx ) {
        QString key( layoutItems[ idx ].to_string().c_str() );

        /** Vertical Layout: Each key will leat to new rows */
        if ( rows.match( key ).hasMatch() ) {
            Hjson::Value row = mLayout[ key.toUtf8().data() ];
            topLyt->addWidget( addRow( row, key, mProperties[ key.toStdString() ], "" ) );
        }

        /** Horizontal Layout: Each key will leat to new columns */
        else if ( cols.match( key ).hasMatch() ) {
            Hjson::Value col = mLayout[ key.toUtf8().data() ];
            topLyt->addWidget( addColumn( col, key, mProperties[ key.toStdString() ], "" ) );
        }

        /** Should not have come here */
        else {
            return topLyt;
        }
    }

    return topLyt;
}


QWidget * QtGreet::LayoutManager::addRow( Hjson::Value row, QString name, Hjson::Value props, QString space ) {
    /** Do we have a row? If yes, we will be expanding the size vertically */
    QRegularExpression rows = QRegularExpression( "^R[0-9]+$" );
    /** Do we have a column? If yes, we will be expanding the size horizontally */
    QRegularExpression cols = QRegularExpression( "^C[0-9]+$" );

    QBoxLayout *lyt = new QBoxLayout( QBoxLayout::LeftToRight );

    lyt->setContentsMargins( getMargins( props[ "Margins" ] ) );
    lyt->setSpacing( getSpacing( props[ "Spacing" ] ) );

    double height = getHeight( props[ "Height" ] );

    if ( height ) {
        if ( height < 1.0 ) {
            lyt->addStrut( height * mScreenSize.height() );
        }

        else {
            lyt->addStrut( (int)height );
        }
    }

    for ( int idx = 0; idx < (int)row.size(); ++idx ) {
        QString      key( row[ idx ].to_string().c_str() );
        Hjson::Value properties( mProperties[ key.toStdString() ] );

        /** Vertical Layout: Each key will leat to new rows */
        if ( rows.match( key ).hasMatch() ) {
            Hjson::Value row = mLayout[ key.toUtf8().data() ];
            lyt->addWidget( addRow( row, key, properties, space + "  " ) );
        }

        /** Horizontal Layout: Each key will leat to new columns */
        else if ( cols.match( key ).hasMatch() ) {
            Hjson::Value col = mLayout[ key.toUtf8().data() ];
            lyt->addWidget( addColumn( col, key, properties, space + "  " ) );
        }

        /** Initialize the widget */
        else {
            // printf( "%s%s ", space.toUtf8().data(), key.toUtf8().data() );

            if ( key == "Stretch" ) {
                lyt->addStretch();
                continue;
            }

            /** We can have multiple widgets with same name (ex: Clock(Time), Clock(Date)) */
            QString widget = key.split( "-" ).at( 0 );

            QWidget *w = WidgetFactory::createWidget( widget, getType( properties[ "Type" ] ), properties );
            lyt->addWidget( w, 0, getAlignment( props[ "Alignment" ] ) );
        }
    }

    // printf( "\n" );

    Container *cw = new Container( props[ "BGColor" ], name );

    cw->setLayout( lyt );

    return cw;
}


QWidget * QtGreet::LayoutManager::addColumn( Hjson::Value col, QString name, Hjson::Value props, QString space ) {
    /** Do we have a row? If yes, we will be expanding the size vertically */
    QRegularExpression rows = QRegularExpression( "^R[0-9]+$" );
    /** Do we have a column? If yes, we will be expanding the size horizontally */
    QRegularExpression cols = QRegularExpression( "^C[0-9]+$" );

    QBoxLayout *lyt = new QBoxLayout( QBoxLayout::TopToBottom );

    lyt->setContentsMargins( getMargins( props[ "Margins" ] ) );
    lyt->setSpacing( getSpacing( props[ "Spacing" ] ) );

    double width = getWidth( props[ "Width" ] );

    if ( width ) {
        if ( width < 1.0 ) {
            lyt->addStrut( width * mScreenSize.width() );
        }

        else {
            lyt->addStrut( (int)width );
        }
    }

    for ( int idx = 0; idx < (int)col.size(); ++idx ) {
        QString      key( col[ idx ].to_string().c_str() );
        Hjson::Value properties = mProperties[ key.toStdString() ];

        /** Vertical Layout: Each key will leat to new rows */
        if ( rows.match( key ).hasMatch() ) {
            Hjson::Value row = mLayout[ key.toUtf8().data() ];
            lyt->addWidget( addRow( row, key, properties, space + "  " ) );
        }

        /** Horizontal Layout: Each key will leat to new columns */
        else if ( cols.match( key ).hasMatch() ) {
            Hjson::Value col = mLayout[ key.toUtf8().data() ];
            lyt->addWidget( addColumn( col, key, properties, space + "  " ) );
        }

        /** Initialize the widget */
        else {
            // qDebug() << (space + key).toUtf8().data();

            if ( key == "Stretch" ) {
                lyt->addStretch();
                continue;
            }

            QString widget = key.split( "-" ).at( 0 );

            QWidget *w = WidgetFactory::createWidget( widget, getType( properties[ "Type" ] ), properties );
            lyt->addWidget( w, 0, getAlignment( props[ "Alignment" ] ) );
        }
    }

    Container *cw = new Container( props[ "BGColor" ], name );

    cw->setLayout( lyt );

    return cw;
}

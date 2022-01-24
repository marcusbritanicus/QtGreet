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

#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "hjson.h"

namespace QtGreet {
    class LayoutManager;
}

class QtGreet::LayoutManager {
    public:
        LayoutManager();
        QBoxLayout * generateLayout( QString layoutFile );

    private:
        /** Screen size */
        QSize mScreenSize;

        /** Original layout file URL */
        QString mLytFile;

        /** Parsed layout file */
        Hjson::Value mLayout;
        Hjson::Value mProperties;

        /** Parse the layout */
        void readLayout( Hjson::Value layout );

        /** Initialize a row and it's widgets */
        QWidget * addRow( Hjson::Value row, QString name, Hjson::Value props, QString space );

        /** Initialize a column and it's widgets */
        QWidget * addColumn( Hjson::Value row, QString name, Hjson::Value props, QString space );
};

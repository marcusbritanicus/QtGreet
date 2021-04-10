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

#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class WidgetFactory {

    public:
        /* Create a widget, given its name, and apply the properties given in the map */
        static QWidget *createWidget( QString name, QString type, QVariantMap properties );

    private:
        /* Most of the properties given can be applied directly, like width and height */
        /* Some properties like text, or icon need to be applied after casting them into suitable types. */
        /* Invalid properties will be ignored. Ex. icon for a label */
        static void applyWidgetProperties( QWidget*, QString, QVariantMap );
};

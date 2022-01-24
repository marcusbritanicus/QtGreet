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

#include "Global.hpp"
#include "hjson.h"

/** Check if the Hjson value is numeric */
bool isNumeric( Hjson::Value val );

/** Convert Hjson::Value to QMargins */
QMargins getMargins( Hjson::Value margins );

/** Convert Hjson::Value to int */
int getSpacing( Hjson::Value spacing );

/** Convert two-char alignment value to Qt::Alignment */
Qt::Alignment getAlignment( Hjson::Value obj );

/** Convert Hjson::Value to width */
int getWidth( Hjson::Value obj );

/** Convert Hjson::Value to height */
int getHeight( Hjson::Value obj );

/** Get the widget type */
QString getType( Hjson::Value type );

/** Get the font weigth from string/number */
QFont::Weight getFontWeight( Hjson::Value weight );

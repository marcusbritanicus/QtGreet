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

#include <QtWidgets>
#include <QtDBus>

class PowerButton : public QToolButton {
    Q_OBJECT;

    public:
        PowerButton( QString type );

        /** Perform a suspend */
        Q_SIGNAL void suspend();

        /** Perform a hibernate */
        Q_SIGNAL void hibernate();

        /**
         * Perform a shutdown or inform the user of a shutdown.
         * @done - The action has been done, just inform the user.
         */
        Q_SIGNAL void shutdown( bool done );

        /**
         * Perform a reboot or inform the user of a reboot.
         * @done - The action has been done, just inform the user.
         */
        Q_SIGNAL void reboot( bool done );
};

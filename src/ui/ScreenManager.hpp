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

#include "Global.hpp"

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
#include <DFL/DF5/Login1.hpp>
#else
#include <DFL/DF6/Login1.hpp>
#endif

class LoginManager;

namespace QtGreet {
    class UI;
    class ScreenManager;
}

class QtGreet::ScreenManager : public QObject {
    Q_OBJECT;

    public:
        ScreenManager();

        /** The parameter @title uniquely identifies a window */
        QScreen * screenForWindow( QString title );

        /** Show the already initialized instances */
        void showInstacnes();

    private:
        /** List of greeters */
        QList<QtGreet::UI *> greeters;

        /** Greeter <-> Screen map */
        QMap<QString, QScreen *> greeterScreenMap;
};

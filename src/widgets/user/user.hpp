/*
	*
	* Copyright 2020 Britanicus <marcusbritanicus@gmail.com>
	*
	* This file is a part of QtGreet project
	* (https://gitlab.com/marcusbritanicus/QtGreet/)
	* Qt5 example of analog clock is the inspiration for this.
	* Some parts of the code are taken from Qt5's example.
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

#include <QtWidgets>

typedef struct user_t {
    QString username;
    uint uid;
    uint gid;
    QString name;
    QString homePath;
    QString shell;
    QPixmap icon;
} User;

Q_DECLARE_METATYPE( User );

typedef QList<User> Users;

class UserName {
    public:
        UserName();

        void switchToNextUser();
        void switchToPreviousUser();

        Users users();
        User currentUser();

    protected:
        Users mUserList;
        int curUser = -1;
};

class UserListCombo: public QComboBox, public UserName {
    Q_OBJECT;

    public:
        UserListCombo();

        void switchToNextUser();
        void switchToPreviousUser();
};

class UserList: public QListWidget, public UserName {
    Q_OBJECT;

    public:
        UserList();

        void switchToNextUser();
        void switchToPreviousUser();
};

class UserNameLabel: public QLabel, public UserName {
    Q_OBJECT;

    public:
        UserNameLabel();

        void switchToNextUser();
        void switchToPreviousUser();
};

class UserNameButton: public QPushButton, public UserName {
    Q_OBJECT;

    public:
        UserNameButton();

        void switchToNextUser();
        void switchToPreviousUser();
};

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

typedef struct user_t {
    QString username;
    uint    uid;
    uint    gid;
    QString name;
    QString homePath;
    QString shell;
    QString icon;
} User;

Q_DECLARE_METATYPE( User );

typedef QList<User> Users;

class UserName {
    public:
        UserName();

        Users users();
        User currentUser();

        virtual void switchToNextUser();
        virtual void switchToPreviousUser();

        /**
         * Set the current user based on the struct user_t.
         * Here we compare the username and user id.
         */
        virtual bool setCurrentUser( User usr );

        /**
         * Set the current user based on the user id.
         * Here we compare only the user id.
         */
        virtual bool setCurrentUser( uint uid );

    protected:
        Users mUserList;
        int curUser = -1;
};

class UserEdit : public QLineEdit, public UserName {
    Q_OBJECT;

    public:
        UserEdit();

        void switchToNextUser();
        void switchToPreviousUser();

        bool setCurrentUser( User usr );
        bool setCurrentUser( uint uid );
};

class UserCombo : public QComboBox, public UserName {
    Q_OBJECT;

    public:
        UserCombo();

        void switchToNextUser();
        void switchToPreviousUser();

        bool setCurrentUser( User usr );
        bool setCurrentUser( uint uid );
};

class UserList : public QListWidget, public UserName {
    Q_OBJECT;

    public:
        UserList();

        void switchToNextUser();
        void switchToPreviousUser();

        bool setCurrentUser( User usr );
        bool setCurrentUser( uint uid );
};

class UserLabel : public QWidget, public UserName {
    Q_OBJECT;

    public:
        UserLabel();

        void setText( QString );

        void setAlignment( Qt::Alignment );

        void switchToNextUser();
        void switchToPreviousUser();

        bool setCurrentUser( User usr );
        bool setCurrentUser( uint uid );

        void setShowNavButtons( bool show );
        void setIconSize( QSize );

        void setFixedHeight( int );

    private:
        QLabel *lbl;
        QToolButton *prevBtn, *nextBtn;
};

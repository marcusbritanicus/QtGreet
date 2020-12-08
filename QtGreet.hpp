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

typedef struct user_t {
    QString username;
    uint uid;
    uint gid;
    QString name;
    QString homePath;
    QString shell;
} User;

typedef struct session_t {
    QString name;
    QString icon;
    QString type;
    QString exec;
} Session;

typedef QList<User> Users;
typedef QList<Session> Sessions;

class QtGreet : public QMainWindow {
    Q_OBJECT;

    public:
        QtGreet();

    private:
        /* Create UI */
        void createUI();

        /* Read the list of users */
        void getUserList();

        /* Read the list of X11/Wayland sessions */
        void getLoginSessions();

        /* Update the username on the screen */
        void updateUser();

        /* Update the session on the screen */
        void updateSess();

        /* Show the validating message */
        void showValidating();

        Users mUserList;
        Sessions mSessions;

        uint curUser = 0;
        uint curSess = 0;

        QLabel *userIcon;
        QPushButton *userName;

        QLabel *clock;

        QToolButton *nextUBtn, *prevUBtn;
        QLineEdit *userPass;

        QToolButton *nextSBtn, *prevSBtn;

        QPushButton *session;
        QLineEdit *sessionCmd;

        QPushButton *loginBtn;

        QImage background;

        QBasicTimer *clockTimer;

        /* User/Session Widgets */
        QWidget *overLay;
        QListWidget *userLW;
        QListWidget *sessionLW;

        /* Validation message */
        QLabel *validating;

    protected:
        void paintEvent( QPaintEvent *pEvent );
        void timerEvent( QTimerEvent *tEvent );

    private Q_SLOTS:
        /* Show the next user */
        void nextUser();

        /* Show the previous user */
        void previousUser();

        /* Show the next user */
        void nextSess();

        /* Show the previous user */
        void previousSess();

        /* User selection */
        void showUserSelection();

        /* Session selection */
        void showSessionSelection();

        /* Try to login */
        void tryLogin();
};

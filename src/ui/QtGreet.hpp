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
#include "user.hpp"
#include "session.hpp"
#include "ThemeManager.hpp"

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
#include <DFL/DF5/Login1.hpp>
#else
#include <DFL/DF6/Login1.hpp>
#endif

class LoginManager;

namespace QtGreet {
    class UI;
}

class QtGreet::UI : public QMainWindow {
    Q_OBJECT;

    public:
        UI();

    private:
        /* Create UI */
        void createUI();

        /* Prepare first use */
        void prepareUIforUse();

        /* Update the username on the screen */
        void updateUser( User );

        /* Update the session on the screen */
        void updateSession( uint );

        /* Show the validating message */
        void showValidating();

        /* Show the validating message */
        void showCustomLogin();

        /* Show the power message */
        void showPowerMessage( bool reboot, bool done );

        User mCurUser;
        Session mCurSession;

        /* Background image */
        QImage background;

        /* QtGreet theme manager */
        ThemeManager *themeManager;

        /* Base to show multiple widgets */
        QStackedWidget *base;

        /* Login manager backend */
        LoginManager *login;

        /* Login1 backend */
        DFL::Login1 *login1;

        /* Validation message */
        QLabel *validating;
        QWidget *cLogin = nullptr;

    protected:
        void paintEvent( QPaintEvent *pEvent );
        void keyPressEvent( QKeyEvent *kEvent );

    private Q_SLOTS:
        // Try to login: If successful, store the username/userid and the session path.
        void tryLogin();

        // Try to login using values from Custom Login widget.
        void tryCustomLogin();

        // ComboBox
        void on_UserCombo_currentIndexChanged( int );

        // ListWidget
        void on_UserList_currentItemChanged( QListWidgetItem *, QListWidgetItem * );

        // UserNavButton - Right - Next User
        void on_UserNavRight_clicked();

        // UserNavButton - Left - Previous User
        void on_UserNavLeft_clicked();

        // SessionNavButton - Right - Next User
        void on_SessionNavRight_clicked();

        // SessionNavButton - Left - Previous User
        void on_SessionNavLeft_clicked();

        // SessionEditButton - Convert the SessionEdit from ReadOnly to ReadWrite
        void on_SessionEditButton_clicked();

        // LoginButton - ToolButton and PushButton - Perform the login: tryLogin()
        void on_LoginButton_clicked();

        // Password - Return pressed - Perform the login: tryLogin()
        void on_Password_returnPressed();

        // SessionEdit - Return pressed - Perform the login: tryLogin()
        void on_SessionEdit_returnPressed();

        // ComboBox
        void on_SessionCombo_currentIndexChanged( int );

        // ListWidget
        void on_SessionList_currentItemChanged( QListWidgetItem *, QListWidgetItem * );

        // PowerButton/Reboot/Halt/Suspend/Hibernate
        void on_PowerButton_suspend();
        void on_PowerButton_hibernate();
        void on_PowerButton_shutdown( bool );
        void on_PowerButton_reboot( bool );
        void on_Suspend_suspend();
        void on_Hibernate_hibernate();
        void on_Halt_shutdown( bool );
        void on_Reboot_reboot( bool );
};

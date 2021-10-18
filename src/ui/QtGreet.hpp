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

#include "Global.hpp"
#include "user.hpp"

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

        uint curUser = 0;
        uint curSess = 0;

        QImage background;

    protected:
        void paintEvent( QPaintEvent *pEvent );
        void keyPressEvent( QKeyEvent *kEvent );

    private Q_SLOTS:

        /* Try to login */
        void tryLogin() {};

        /* Auto Slots */

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

        // Generic okay button - Not implemented
        // void on_OkayButton_clicked();
        // Generic cancel button - Not implemented
        // void on_CancelButton_clicked();

        // SessionEditButton - Convert the SessionEdit from ReadOnly to ReadWrite
        void on_SessionEditButton_clicked();
        // LoginButton - ToolButton and PushButton - Perform the login: tryLogin()
        void on_LoginButton_clicked();

        // PushButton
        void on_SessionName_clicked();
        // ComboBox
        void on_SessionName_currentIndexChanged( int );
        // ListWidget
        void on_SessionName_currentItemChanged( QListWidgetItem *, QListWidgetItem * );
};

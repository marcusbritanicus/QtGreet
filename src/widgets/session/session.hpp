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

typedef struct session_t {
    QString name;
    QString icon;
    QString type;
    QString exec;
    QString file;
} Session;
Q_DECLARE_METATYPE( Session );

typedef QList<Session> Sessions;

class SessionEdit : public QLineEdit {
    Q_OBJECT;

    public:
        SessionEdit();
};

class SessionName {
    public:
        SessionName( bool );

        void switchToNextSession();
        void switchToPreviousSession();

        Sessions sessions();
        Session currentSession();

    protected:
        Sessions mSessionList;
        int curSess = 0;
};

class SessionCombo: public QComboBox, public SessionName {
    Q_OBJECT;

    public:
        SessionCombo( bool );

        void switchToNextSession();
        void switchToPreviousSession();
};

class SessionList : public QListWidget, public SessionName {
    Q_OBJECT;

    public:
        SessionList( bool );

        void switchToNextSession();
        void switchToPreviousSession();
};

class SessionLabel: public QWidget, public SessionName {
    Q_OBJECT;

    public:
        SessionLabel( bool );

        void setText( QString );

        void setAlignment( Qt::Alignment );

        void switchToNextSession();
        void switchToPreviousSession();

        void setShowNavButtons( bool show );

        void setIconSize( QSize );

        void setFixedHeight( int );

    private:
        QLabel *lbl;
        QToolButton *prevBtn, *nextBtn;
};

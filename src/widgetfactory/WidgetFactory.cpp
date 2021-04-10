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

#include "WidgetFactory.hpp"

/* Individual widget headers */

// Provides Analog, Digital and Simple clock
#include "ClockWidget.hpp"

QWidget *WidgetFactory::createWidget( QString name, QString type, QVariantMap properties ) {

    if ( name == "QWidget" ) {
        // qWarning() << "Creating QWidget";

        QWidget *w = new QWidget();
        applyWidgetProperties( w, type, properties );

        if ( type == "Panel" )
            w->setObjectName( "Panel" );

        else if ( type == "Sidebar" )
            w->setObjectName( "Sidebar" );

        return w;
    }

    else if ( name == "QLabel" ) {
        // qWarning() << "Creating QLabel";

        QLabel *w = new QLabel();
        applyWidgetProperties( w, "Label", properties );

        return w;
    }

    else if ( name == "Clock" ) {

        if ( type == "Analog" ) {
            // qWarning() << "Analog clock";

            AnalogClock *cw = new AnalogClock( "#ffffff" );
            applyWidgetProperties( cw, type, properties );

            return cw;
        }

        if ( type == "Digital" ) {
            // qWarning() << "Digital clock";

            DigitalClock *cw = new DigitalClock();
            applyWidgetProperties( cw, type, properties );

            return cw;
        }

        else {
            // qWarning() << "Simple clock";

            SimpleClock *cw = new SimpleClock();
            cw->setFixedHeight( 40 );
            applyWidgetProperties( cw, type, properties );

            return cw;
        }
    }

    else if ( name == "UserIcon" ) {
        // qWarning() << "UserIcon";

        QLabel *lbl = new QLabel();
        lbl->setObjectName( "userIcon" );
        lbl->setAlignment( Qt::AlignCenter );
        lbl->setFixedSize( QSize( 72, 72 ) );
        lbl->setPixmap( QIcon( ":/icons/user.png" ).pixmap( QSize( 64, 64 ) ) );
        applyWidgetProperties( lbl, type, properties );

        return lbl;
    }

    else if ( name == "UserName" ) {
        // qWarning() << "User Name";

        if ( type == "LineEdit" ) {
            QLineEdit *le = new QLineEdit();
            applyWidgetProperties( le, type, properties );

            return le;
        }

        else if ( type == "Label" ) {

            QLabel *lbl = new QLabel();
            lbl->setFixedHeight( 27 );
            lbl->setText( "Hello, Marcus" );
            applyWidgetProperties( lbl, type, properties );

            return lbl;
        }

        else if ( type == "PushButton" ) {

            QPushButton *btn = new QPushButton();
            btn->setFixedHeight( 27 );
            btn->setIcon( QIcon( ":/icons/session.png" ) );
            btn->setText( "DesQ (Wayland)" );
            applyWidgetProperties( btn, type, properties );

            return btn;
        }
    }

    else if ( name == "UserList" ) {
        // qWarning() << "User list";

        if ( type == "List" ) {
            QListWidget *lw = new QListWidget();
            lw->setIconSize( QSize( 36, 36 ) );
            lw->setFont( QFont( "Quicksand", 12 ) );
            lw->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
            lw->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
            applyWidgetProperties( lw, type, properties );

            return lw;
        }

        else if ( type == "Combo" ) {
            QComboBox *cb = new QComboBox();
            applyWidgetProperties( cb, type, properties );

            return cb;
        }
    }

    else if ( name == "Password" ) {
        // qWarning() << "Password";

        QLineEdit *le = new QLineEdit();
        le->setObjectName( "Password" );
        le->setEchoMode( QLineEdit::Password );
        le->setFixedHeight( 27 );
        applyWidgetProperties( le, type, properties );

        return le;
    }

    else if ( name == "UserPass" ) {
        // qWarning() << "Password";

        QLabel *lbl = new QLabel();
        lbl->setFont( QFont( "Quicksand", 12, QFont::Bold ) );
        lbl->setText( "Hello, Marcus" );
        lbl->setAlignment( Qt::AlignCenter );

        QLineEdit *le = new QLineEdit();
        le->setObjectName( "Password" );
        le->setEchoMode( QLineEdit::Password );
        le->setFixedHeight( 27 );
        applyWidgetProperties( le, type, properties );

        QToolButton *btn = new QToolButton();
        btn->setIcon( QIcon::fromTheme( ":/icons/arrow-right.png" ) );
        btn->setIconSize( QSize( 22, 22 ) );
        btn->setFixedSize( QSize( 27, 27 ) );

        QHBoxLayout *hlyt = new QHBoxLayout();
        hlyt->setContentsMargins( QMargins() );
        hlyt->setSpacing( 0 );
        hlyt->addWidget( le );
        hlyt->addWidget( btn );

        QWidget *w = new QWidget();
        QVBoxLayout *lyt = new QVBoxLayout();
        lyt->setContentsMargins( QMargins() );
        lyt->setSpacing( 15 );
        lyt->addStretch();
        lyt->addWidget( lbl );
        lyt->addLayout( hlyt );
        lyt->addStretch();
        w->setLayout( lyt );

        return w;
    }

    else if ( name == "SessionName" ) {
        // qWarning() << "Session name";

        if ( type == "LineEdit" ) {
            QLineEdit *le = new QLineEdit();
            applyWidgetProperties( le, type, properties );

            return le;
        }

        else if ( type == "Label" ) {

            QLabel *lbl = new QLabel();
            lbl->setFixedHeight( 27 );
            lbl->setText( "DesQ (Wayland)" );
            applyWidgetProperties( lbl, type, properties );

            return lbl;
        }

        else if ( type == "PushButton" ) {

            QPushButton *btn = new QPushButton();
            btn->setFixedHeight( 27 );
            btn->setIcon( QIcon( ":/icons/session.png" ) );
            btn->setText( "DesQ (Wayland)" );
            applyWidgetProperties( btn, type, properties );

            return btn;
        }
    }

    else if ( name == "SessionEdit" ) {
        // qWarning() << "Session Edit";

        QLineEdit *le = new QLineEdit();
        le->setObjectName( "Password" );
        le->setFixedHeight( 27 );
        applyWidgetProperties( le, type, properties );

        return le;
    }

    else if ( name == "SessionList" ) {
        // qWarning() << "Session List";

        if ( type == "List" ) {
            QListWidget *lw = new QListWidget();
            lw->setIconSize( QSize( 36, 36 ) );
            lw->setFont( QFont( "Quicksand", 12 ) );
            lw->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
            lw->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
            applyWidgetProperties( lw, type, properties );

            QStringList sessions = {
                "DesQ (Wayland)", "Paper (Wayland)", "Wayfire (Wayland)", "Sway (Wayland)", "Plasma Wayland", "Weston (Wayland)",
                "Awesome (X11)", "I3-with-shmlog (X11)", "LXQt (X11)", "MWM (X11)", "Oyster (X11)", "TinyWM (X11)", "XFCE (X11)",
                "Enlightenment (X11)", "I3 (X11)", "MatchBox (X11)", "OpenBox (X11)", "Plasma (X11)", "Win7 (X11)"
            };

            for( QString sess: sessions ) {
                QListWidgetItem *item = new QListWidgetItem( QIcon( ":/icons/session.png" ), sess, lw );
                lw->addItem( item );
            }

            return lw;
        }

        else if ( type == "Combo" ) {
            QComboBox *cb = new QComboBox();
            applyWidgetProperties( cb, type, properties );

            return cb;
        }
    }

    else if ( name == "SessionEditButton" ) {
        // qWarning() << "SessionEdit Button";

        QToolButton *nav = new QToolButton();
        nav->setFixedSize( QSize( 27, 27 ) );
        nav->setIconSize( QSize( 22, 22 ) );
        nav->setIcon( QIcon( ":/icons/edit.png" ) );
        nav->setAutoRaise( true );
        applyWidgetProperties( nav, type, properties );

        return nav;
    }

    else if ( name == "PowerButton" ) {
        // qWarning() << "PowerButton";

        if ( type == "Menu" ) {
            QToolButton *btn = new QToolButton();
            btn->setAutoRaise( true );
            btn->setFixedSize( QSize( 40, 40 ) );
            btn->setIconSize( QSize( 32, 32 ) );
            btn->setIcon( QIcon( ":/icons/shutdown.png" ) );
            applyWidgetProperties( btn, type, properties );

            QMenu *menu = new QMenu();
            menu->addAction( QIcon( ":/icons/suspend.png" ), "Suspend to RAM" );
            menu->addAction( QIcon( ":/icons/hibernate.png" ), "Suspend to Disk" );
            menu->addAction( QIcon( ":/icons/shutdown.png" ), "Halt System" );
            menu->addAction( QIcon( ":/icons/reboot.png" ), "Reboot System" );

            btn->setMenu( menu );
            btn->setPopupMode( QToolButton::InstantPopup );

            return btn;
        }

        else if ( type == "Halt" ) {
            QToolButton *btn = new QToolButton();
            btn->setObjectName( "halt" );
            btn->setAutoRaise( true );
            btn->setFixedSize( QSize( 40, 40 ) );
            btn->setIconSize( QSize( 32, 32 ) );
            btn->setIcon( QIcon( ":/icons/shutdown.png" ) );
            applyWidgetProperties( btn, type, properties );

            return btn;
        }

        else if ( type == "Reboot" ) {
            QToolButton *btn = new QToolButton();
            btn->setObjectName( "reboot" );
            btn->setAutoRaise( true );
            btn->setFixedSize( QSize( 40, 40 ) );
            btn->setIconSize( QSize( 32, 32 ) );
            btn->setIcon( QIcon( ":/icons/reboot.png" ) );
            applyWidgetProperties( btn, type, properties );

            return btn;
        }
    }

    else if ( name == "NumLock" ) {
        // qWarning() << "NumLock";

        QLabel *lbl = new QLabel( "<b>#</b>" );
        lbl->setFixedSize( QSize( 40, 40 ) );
        QFile num( "/sys/class/leds/input0::numlock/brightness" );
        if ( num.open( QFile::ReadOnly ) ) {
            if ( num.readAll().toInt() )
                lbl->setStyleSheet( "color: rgb(255, 255, 255)" );

            else
                lbl->setStyleSheet( "color: rgba(255, 255, 255, 50)" );

            num.close();
        }

        else
            lbl->setStyleSheet( "color: rgba(255, 255, 255, 20)" );

        applyWidgetProperties( lbl, type, properties );
        return lbl;
    }

    else if ( name == "CapsLock" ) {
        // qWarning() << "CapsLock";

        QLabel *lbl = new QLabel( "<b>A</b>" );
        lbl->setFixedSize( QSize( 40, 40 ) );
        QFile caps( "/sys/class/leds/input0::capslock/brightness" );
        if ( caps.open( QFile::ReadOnly ) ) {
            if ( caps.readAll().toInt() )
                lbl->setStyleSheet( "color: rgb(255, 255, 255)" );

            else
                lbl->setStyleSheet( "color: rgba(255, 255, 255, 50)" );

            caps.close();
        }

        else
            lbl->setStyleSheet( "color: rgba(255, 255, 255, 20)" );

        return lbl;
    }

    else if ( name == "UserNav" ) {
        // qWarning() << "UserNav";

        QToolButton *nav = new QToolButton();
        nav->setObjectName( "nav" );
        nav->setFixedSize( QSize( 27, 27 ) );
        nav->setIconSize( QSize( 22, 22 ) );
        nav->setAutoRaise( true );
        if ( type == "Right" )
            nav->setIcon( QIcon( ":/icons/arrow-right.png" ) );

        else
            nav->setIcon( QIcon( ":/icons/arrow-left.png" ) );

        applyWidgetProperties( nav, type, properties );

        return nav;
    }

    else if ( name == "SessionNav" ) {
        // qWarning() << "SessionNav";

        QToolButton *nav = new QToolButton();
        nav->setObjectName( "nav" );
        nav->setFixedSize( QSize( 27, 27 ) );
        nav->setIconSize( QSize( 22, 22 ) );
        nav->setAutoRaise( true );
        if ( type == "Right" )
            nav->setIcon( QIcon( ":/icons/arrow-right.png" ) );

        else
            nav->setIcon( QIcon( ":/icons/arrow-left.png" ) );

        applyWidgetProperties( nav, type, properties );

        return nav;
    }

    else if ( name == "Logo" ) {
        // qWarning() << "Logo";

        QLabel *logo = new QLabel();
        logo->setFixedSize( 40, 40 );
        logo->setPixmap( QIcon( ":/icons/qtgreet1.png" ).pixmap( 40 ) );
        applyWidgetProperties( logo, type, properties );

        return logo;
    }

    else if ( name == "LoginButton" ) {
        // qWarning() << "LoginButton";

        if ( type == "ToolButton" ) {
            QToolButton *btn = new QToolButton();
            btn->setFixedSize( QSize( 27, 27 ) );
            btn->setIconSize( QSize( 22, 22 ) );
            btn->setIcon( QIcon( ":/icons/arrow-right.png" ) );
            btn->setAutoRaise( true );
            applyWidgetProperties( btn, type, properties );

            return btn;
        }

        else {
            QPushButton *btn = new QPushButton();
            btn->setFixedHeight( 27 );
            btn->setIconSize( QSize( 22, 22 ) );
            btn->setIcon( QIcon( ":/icons/arrow-right.png" ) );
            btn->setText( "&Login" );
            btn->setFlat( true );
            applyWidgetProperties( btn, type, properties );

            return btn;
        }
    }

    else if ( name == "Separator" ) {
        // qWarning() << "Separator";

        QWidget *sep = new QWidget();
        sep->setStyleSheet( "background-color: gray;" );

        if ( type == "Horizontal" ) {       // To be added in a QHBoxLayout()
            sep->setFixedWidth( 1 );
            sep->setMinimumHeight( 1 );
        }

        else {                              // To be added in a QVBoxLayout()
            sep->setFixedHeight( 1 );
            sep->setMinimumWidth( 1 );
        }

        return sep;
    }

    else {

        // qWarning() << name << type;
    }

    return new QWidget();
};

void WidgetFactory::applyWidgetProperties( QWidget *widget, QString type, QVariantMap properties ) {

    for( QString key: properties.keys() ) {
        /* Width */
        if ( key == "Width" )
            widget->resize( properties[ key ].toInt(), widget->height() );

        else if ( key == "MinimumWidth" )
            widget->setMinimumWidth( properties[ key ].toInt() );

        else if ( key == "MaximumWidth" )
            widget->setMaximumWidth( properties[ key ].toInt() );

        else if ( key == "FixedWidth" )
            widget->setFixedWidth( properties[ key ].toInt() );

        /* Height */
        else if ( key == "Height" )
            widget->resize( widget->width(), properties[ key ].toInt() );

        else if ( key == "MinimumHeight" )
            widget->setMinimumHeight( properties[ key ].toInt() );

        else if ( key == "MaximumHeight" )
            widget->setMaximumHeight( properties[ key ].toInt() );

        else if ( key == "FixedHeight" )
            widget->setFixedHeight( properties[ key ].toInt() );

        /* Text */
        else if ( key == "Text" ) {
            if ( type == "Label" )
                qobject_cast<QLabel*>( widget )->setText( properties[ key ].toString() );

            else if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setText( properties[ key ].toString() );

            else if ( type == "ToolButton" )
                qobject_cast<QToolButton*>( widget )->setText( properties[ key ].toString() );
        }

        /* Icon */
        else if ( key == "Icon" ) {
            if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setIcon( QIcon( properties[ key ].toString() ) );

            else if ( type == "ToolButton" )
                qobject_cast<QToolButton*>( widget )->setIcon( QIcon( properties[ key ].toString() ) );
        }

        /* Theme Icon */
        else if ( key == "ThemeIcon" ) {
            if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setIcon( QIcon::fromTheme( properties[ key ].toString() ) );

            else if ( type == "ToolButton" )
                qobject_cast<QToolButton*>( widget )->setIcon( QIcon::fromTheme( properties[ key ].toString() ) );
        }
    }
}

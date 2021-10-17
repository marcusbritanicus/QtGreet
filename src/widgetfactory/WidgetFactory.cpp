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
#include "buttons.hpp"
#include "labels.hpp"
#include "power.hpp"
#include "session.hpp"
#include "user.hpp"

const QSettings sett( "QtGreet", "QtGreet" );

QWidget *WidgetFactory::createWidget( QString name, QString type, Hjson::Value properties ) {

    if ( name == "QWidget" ) {
        // qWarning() << "Creating QWidget";

        QWidget *w = new QWidget();
        applyWidgetProperties( w, name, type, properties );
        w->setObjectName( type );

        return w;
    }

    else if ( name == "QLabel" ) {
        // qWarning() << "Creating QLabel";

        QLabel *w = new QLabel();
        applyWidgetProperties( w, "QLabel", "Label", properties );

        return w;
    }

    else if ( name == "Battery" ) {

        QLabel *w = new QLabel();
        w->setObjectName( "Battery" );
        w->setPixmap( QIcon::fromTheme( "battery-full-symbolic" ).pixmap( 24 ) );
        w->resize( 24, 24 );
        applyWidgetProperties( w, name, type, properties );

        return w;
    }

    else if ( name == "Clock" ) {

        if ( type == "Analog" ) {
            // qWarning() << "Analog clock";

            AnalogClock *cw = new AnalogClock( "#ffffff" );
            cw->setObjectName( "Clock" );
            applyWidgetProperties( cw, name, type, properties );

            return cw;
        }

        else if ( type == "Digital" ) {
            // qWarning() << "Digital clock";

            DigitalClock *cw = new DigitalClock();
            cw->setObjectName( "Clock" );
            applyWidgetProperties( cw, name, type, properties );

            return cw;
        }

        else {
            // qWarning() << "Simple clock";

            bool time = ( type == "Time" );
            bool date = ( type == "Date" );

            if ( not date and not time ) {
                date = true;
                time = true;
            }

            SimpleClock *cw = new SimpleClock( time, date );
            cw->setObjectName( "Clock" );
            applyWidgetProperties( cw, name, type, properties );

            return cw;
        }
    }

    else if ( name == "UserIcon" ) {
        // qWarning() << "UserIcon";

        UserIcon *lbl = new UserIcon();
        applyWidgetProperties( lbl, name, type, properties );

        return lbl;
    }

    else if ( name == "UserName" ) {
        // qWarning() << "User Name";

        if ( type == "LineEdit" ) {
            QLineEdit *le = new QLineEdit();
            applyWidgetProperties( le, name, type, properties );

            return le;
        }

        else if ( type == "Label" ) {

            UserNameLabel *lbl = new UserNameLabel();
            applyWidgetProperties( lbl, name, type, properties );

            return lbl;
        }

        else if ( type == "PushButton" ) {

            UserNameButton *btn = new UserNameButton();
            applyWidgetProperties( btn, name, type, properties );

            return btn;
        }

        else if ( type == "List" ) {
            UserList *lw = new UserList();
            applyWidgetProperties( lw, name, type, properties );

            return lw;
        }

        else if ( type == "Combo" ) {
            UserListCombo *cb = new UserListCombo();
            applyWidgetProperties( cb, name, type, properties );

            return cb;
        }
    }

    else if ( name == "Password" ) {
        // qWarning() << "Password";

        QLineEdit *le = new QLineEdit();
        le->setObjectName( "Password" );
        le->setPlaceholderText( "Password" );
        le->setEchoMode( QLineEdit::Password );
        le->setFixedHeight( 27 );
        le->setAlignment( Qt::AlignCenter );
        applyWidgetProperties( le, name, type, properties );

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
        applyWidgetProperties( le, name, type, properties );

        QToolButton *btn = new QToolButton();
        btn->setIcon( QIcon::fromTheme( ":/icons/arrow-right.png" ) );
        btn->setIconSize( QSize( 22, 22 ) );
        btn->resize( QSize( 27, 27 ) );

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

        bool custom = sett.value( "AllowCustomSessions" ).toBool();

        if ( type == "List" ) {
            SessionList *lw = new SessionList( custom );
            applyWidgetProperties( lw, name, type, properties );

            return lw;
        }

        else if ( type == "Combo" ) {
            SessionListCombo *cb = new SessionListCombo( custom );
            applyWidgetProperties( cb, name, type, properties );

            return cb;
        }

        else if ( type == "Label" ) {
            SessionNameLabel *sess = new SessionNameLabel( custom );
            applyWidgetProperties( sess, name, type, properties );

            return sess;
        }

        else if ( type == "PushButton" ) {

            SessionNameButton *btn = new SessionNameButton( custom );
            applyWidgetProperties( btn, name, type, properties );

            return btn;
        }

        else if ( type == "ToolButton" ) {

            QToolButton *btn = new QToolButton();
            btn->setIcon( QIcon( ":/icons/session.png" ) );
            applyWidgetProperties( btn, name, type, properties );

            return btn;
        }
    }

    else if ( name == "SessionEdit" ) {
        // qWarning() << "Session Edit";

        SessionEdit *le = new SessionEdit();
        applyWidgetProperties( le, name, type, properties );

        return le;
    }

    else if ( name == "SessionEditButton" ) {
        // qWarning() << "SessionEdit Button";

        SessionEditButton *editBtn = new SessionEditButton();
        applyWidgetProperties( editBtn, name, type, properties );

        return editBtn;
    }

    else if ( name == "PowerButton" ) {
        // qWarning() << "PowerButton";

        PowerButton *btn = new PowerButton( type );
        btn->setObjectName( "PowerButton" );
        applyWidgetProperties( btn, name, type, properties );

        return btn;
    }

    else if ( ( name == "CapsLock" ) or ( name == "NumLock" ) ) {
        // qWarning() << "CapsLock";

        LockState *lock = new LockState( name );
        applyWidgetProperties( lock, name, type, properties );

        return lock;
    }

    else if ( ( name == "UserNav" ) or ( name == "SessionNav" ) ) {

        NavButton *nav = new NavButton( name, type );
        applyWidgetProperties( nav, name, type, properties );

        return nav;
    }

    else if ( name == "Logo" ) {
        // qWarning() << "Logo";

        Logo *logo = new Logo();
        applyWidgetProperties( logo, name, type, properties );

        return logo;
    }

    else if ( name == "LoginButton" ) {
        // qWarning() << "LoginButton";

        if ( type == "ToolButton" ) {
            LoginToolButton *btn = new LoginToolButton();
            applyWidgetProperties( btn, name, type, properties );

            return btn;
        }

        else {
            LoginPushButton *btn = new LoginPushButton();
            applyWidgetProperties( btn, name, type, properties );

            return btn;
        }
    }

    else if ( name == "VirtualKeyboard" ) {

        QToolButton *w = new QToolButton();
        w->setIcon( QIcon::fromTheme( "input-keyboard-virtual", QIcon::fromTheme( "input-keyboard" ) ) );
        w->setObjectName( "VirtualKeyboard" );
        w->resize( 24, 24 );
        applyWidgetProperties( w, name, type, properties );

        return w;
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

        qWarning() << name << type;
    }

    return new QWidget();
};

void WidgetFactory::applyWidgetProperties( QWidget *widget, QString name, QString type, Hjson::Value properties ) {

    for( size_t i = 0; i < properties.size(); i++ ) {
        QString key( properties.key( i ).c_str() );

        /* Width */
        if ( key == "Width" ) {
            double width = properties[ key.toStdString() ].to_double();
            if ( width <= 1.0 )
                width *= mScreenSize.width();

            widget->setFixedWidth( width );
        }

        else if ( key == "MinimumWidth" ) {
            double width = properties[ key.toStdString() ].to_double();
            if ( width <= 1.0 )
                width *= mScreenSize.width();

            widget->setMinimumWidth( width );
        }

        else if ( key == "MaximumWidth" ) {
            double width = properties[ key.toStdString() ].to_double();
            if ( width <= 1.0 )
                width *= mScreenSize.width();

            widget->setMaximumWidth( width );
        }

        else if ( key == "FixedWidth" ) {
            double width = properties[ key.toStdString() ].to_double();
            if ( width <= 1.0 )
                width *= mScreenSize.width();

            widget->setFixedWidth( width );
        }

        /* Height */
        else if ( key == "Height" ) {
            double height = properties[ key.toStdString() ].to_double();
            if ( height <= 1.0 )
                height *= mScreenSize.height();

            widget->setFixedHeight( height );
        }

        else if ( key == "MinimumHeight" ) {
            double height = properties[ key.toStdString() ].to_double();
            if ( height <= 1.0 )
                height *= mScreenSize.height();

            widget->setMinimumHeight( height );
        }

        else if ( key == "MaximumHeight" ) {
            double height = properties[ key.toStdString() ].to_double();
            if ( height <= 1.0 )
                height *= mScreenSize.height();

            widget->setMaximumHeight( height );
        }

        else if ( key == "FixedHeight" ) {
            double height = properties[ key.toStdString() ].to_double();
            if ( height <= 1.0 )
                height *= mScreenSize.height();

            widget->setFixedHeight( height );
        }

        /* Text */
        else if ( key == "Text" ) {
            if ( type == "Label" )
                qobject_cast<QLabel*>( widget )->setText( properties[ key.toStdString() ].to_string().c_str() );

            else if ( type == "Combo" )
                qobject_cast<QComboBox*>( widget )->setCurrentText( properties[ key.toStdString() ].to_string().c_str() );

            else if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setText( properties[ key.toStdString() ].to_string().c_str() );

            else if ( type == "ToolButton" )
                qobject_cast<QToolButton*>( widget )->setText( properties[ key.toStdString() ].to_string().c_str() );
        }

        /* Icon */
        else if ( key == "Icon" ) {
            if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setIcon( QIcon( properties[ key.toStdString() ].to_string().c_str() ) );

            else if ( type == "ToolButton" )
                qobject_cast<QToolButton*>( widget )->setIcon( QIcon( properties[ key.toStdString() ].to_string().c_str() ) );

            else if ( name == "Logo" )
                qobject_cast<QToolButton*>( widget )->setIcon   ( QIcon( properties[ key.toStdString() ].to_string().c_str() ) );
        }

        /* Theme Icon */
        else if ( key == "ThemeIcon" ) {
            if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setIcon( QIcon::fromTheme( properties[ key.toStdString() ].to_string().c_str() ) );

            else if ( ( type == "ToolButton" ) or ( name == "PowerButton" ) )
                qobject_cast<QToolButton*>( widget )->setIcon( QIcon::fromTheme( properties[ key.toStdString() ].to_string().c_str() ) );
        }

        /* Font */
        else if ( key == "Font" ) {
            QStringList fontBits = QString( properties[ key.toStdString() ].to_string().c_str() ).split( ", ", Qt::SkipEmptyParts );
            QFont font(
                fontBits[ 0 ],
                fontBits[ 1 ].toInt(),
                ( fontBits[ 2 ] == "Bold" ? QFont::Bold : QFont::Normal ),
                ( fontBits[ 3 ] == "Italic" )
            );
            widget->setFont( font );
        }

        /* FontSize */
        else if ( key == "FontSize" ) {
            QFont font( widget->font() );

            double fs = properties[ key.toStdString() ].to_double();
            if ( properties[ key.toStdString() ].type() == Hjson::Type::Double )
                fs *= font.pointSize();

            font.setPointSize( (int)fs );
            widget->setFont( font );
        }

        /* Format string for Clock */
        else if ( name == "Clock" and key == "Format" ) {
            if ( type == "Time" )
                qobject_cast<SimpleClock *>( widget )->setTimeFormat( properties[ key.toStdString() ].to_string().c_str() );

            else if ( type == "Date" )
                qobject_cast<SimpleClock *>( widget )->setDateFormat( properties[ key.toStdString() ].to_string().c_str() );
        }

        else if ( key == "IconSize" ) {
            QSize iSize = QSize( properties[ key.toStdString() ].to_int64(), properties[ key.toStdString() ].to_int64() );

            if ( type == "List" )
                qobject_cast<QListWidget *>( widget  )->setIconSize( iSize );

            else if ( ( type == "PowerButton" ) or ( type == "ToolButton" ) )
                qobject_cast<QToolButton*>( widget )->setIconSize( iSize );

            else if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setIconSize( iSize );

            else if ( name == "Logo" )
                qobject_cast<Logo*>( widget )->setIconSize( iSize );
        }

        else if ( key == "TextAlign" ) {
            if ( name == "QLabel" or type == "Label" ) {
                if ( properties[ key.toStdString() ].to_string() == "Center" )
                    qobject_cast<QLabel *>( widget )->setAlignment( Qt::AlignCenter );

                else if ( properties[ key.toStdString() ].to_string() == "Left" )
                    qobject_cast<QLabel *>( widget )->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

                else if ( properties[ key.toStdString() ].to_string() == "Right" )
                    qobject_cast<QLabel *>( widget )->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
            }
        }

        else if ( key == "ToolTip" ) {
            widget->setToolTip( properties[ key.toStdString() ].to_string().c_str() );
        }
    }
}

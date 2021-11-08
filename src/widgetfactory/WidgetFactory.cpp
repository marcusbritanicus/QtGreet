/**
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
**/

#include "WidgetFactory.hpp"
#include "LayoutUtils.hpp"

/** Individual widget headers */

// Provides Analog, Digital and Simple clock
#include "ClockWidget.hpp"
#include "buttons.hpp"
#include "labels.hpp"
#include "power.hpp"
#include "session.hpp"
#include "user.hpp"

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
            le->setObjectName( "UserEdit" );
            applyWidgetProperties( le, name, type, properties );

            return le;
        }

        else if ( type == "Label" ) {

            UserLabel *lbl = new UserLabel();
            applyWidgetProperties( lbl, name, type, properties );

            return lbl;
        }

        else if ( type == "List" ) {
            UserList *lw = new UserList();
            applyWidgetProperties( lw, name, type, properties );

            return lw;
        }

        else if ( type == "Combo" ) {
            UserCombo *cb = new UserCombo();
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
        le->setAlignment( Qt::AlignCenter );
        applyWidgetProperties( le, name, type, properties );

        return le;
    }

    else if ( name == "SessionName" ) {
        // qWarning() << "Session name";

        bool custom = sett->value( "AllowCustomSessions" ).toBool();

        if ( type == "List" ) {
            SessionList *lw = new SessionList( custom );
            applyWidgetProperties( lw, name, type, properties );

            return lw;
        }

        else if ( type == "Combo" ) {
            SessionCombo *cb = new SessionCombo( custom );
            applyWidgetProperties( cb, name, type, properties );

            return cb;
        }

        else if ( type == "Label" ) {
            SessionLabel *sess = new SessionLabel( custom );
            applyWidgetProperties( sess, name, type, properties );

            return sess;
        }

        else if ( type == "ToolButton" ) {

            QToolButton *btn = new QToolButton();
            btn->setObjectName( "SessionName" );
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
        std::string stdKey( properties.key( i ) );
        QString key( properties.key( i ).c_str() );

        /** Width */
        if ( key.contains( "Width" ) ) {
            int w = getWidth( properties[ stdKey ] );

            if ( key == "MinimumWidth" )
                widget->setMinimumWidth( w );

            else if ( key == "MaximumWidth" )
                widget->setMaximumWidth( w );

            else
                widget->setFixedWidth( w );
        }

        else if ( key.contains( "Height" ) ) {
            int h = getHeight( properties[ stdKey ] );

            if ( key == "MinimumHeight" )
                widget->setMinimumHeight( h );

            else if ( key == "MaximumHeight" )
                widget->setMaximumHeight( h );

            else {
                if ( name == "UserName" and type == "Label" )
                    qobject_cast<UserLabel*>( widget )->setFixedHeight( h );

                else if ( name == "SessionName" and type == "Label" )
                    qobject_cast<SessionLabel*>( widget )->setFixedHeight( h );

                else
                    widget->setFixedHeight( h );
            }
        }

        /** Text */
        else if ( key == "Text" ) {
            QString text( properties[ stdKey ].to_string().c_str() );

            if ( name == "UserName" and type == "Label" )
                qobject_cast<UserLabel*>( widget )->setText( text );

            else if ( name == "SessionName" and type == "Label" )
                qobject_cast<SessionLabel*>( widget )->setText( text );

            else if ( type == "Label" )
                qobject_cast<QLabel*>( widget )->setText( text );

            else if ( type == "Combo" )
                qobject_cast<QComboBox*>( widget )->setCurrentText( text );

            else if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setText( text );

            else if ( type == "ToolButton" )
                qobject_cast<QToolButton*>( widget )->setText( text );
        }

        /** Icon */
        else if ( key == "Icon" ) {
            QIcon icn( properties[ stdKey ].to_string().c_str() );
            if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setIcon( icn );

            else if ( type == "ToolButton" )
                qobject_cast<QToolButton*>( widget )->setIcon( icn );

            else if ( name == "Logo" )
                qobject_cast<QToolButton*>( widget )->setIcon   ( icn );
        }

        /** Theme Icon */
        else if ( key == "ThemeIcon" ) {
            QIcon icn = QIcon::fromTheme( properties[ stdKey ].to_string().c_str() );
            if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setIcon( icn );

            else if ( ( type == "ToolButton" ) or ( name == "PowerButton" ) )
                qobject_cast<QToolButton*>( widget )->setIcon( icn );
        }

        /** Font */
        else if ( key == "Font" ) {
            QStringList fontBits = QString( properties[ stdKey ].to_string().c_str() ).split( ", ", Qt::SkipEmptyParts );
            QFont font(
                fontBits[ 0 ],
                fontBits[ 1 ].toInt(),
                ( fontBits[ 2 ] == "Bold" ? QFont::Bold : QFont::Normal ),
                ( fontBits[ 3 ] == "Italic" )
            );
            widget->setFont( font );
        }

        /** FontSize - Just to scale the font size */
        else if ( key == "FontSize" ) {
            QFont font( widget->font() );

            double fs = properties[ stdKey ].to_double();
            if ( properties[ stdKey ].type() == Hjson::Type::Double )
                fs *= font.pointSize();

            font.setPointSize( (int)fs );
            widget->setFont( font );
        }

        /** FontWeight - Just to change font weigth */
        else if ( key == "FontWeight" ) {
            QFont font( widget->font() );

            font.setWeight( getFontWeight( properties[ stdKey ] ) );
            widget->setFont( font );
        }

        /** Italic - Just to change font italicity */
        else if ( key == "FontItalic" ) {
            QFont font( widget->font() );

            font.setItalic( (bool)properties[ stdKey ] );
            widget->setFont( font );
        }

        /** Format string for Clock */
        else if ( name == "Clock" and key == "Format" ) {
            if ( type == "Time" )
                qobject_cast<SimpleClock *>( widget )->setTimeFormat( properties[ stdKey ].to_string().c_str() );

            else if ( type == "Date" )
                qobject_cast<SimpleClock *>( widget )->setDateFormat( properties[ stdKey ].to_string().c_str() );
        }

        /** Icon Size */
        else if ( key == "IconSize" ) {
            QSize iSize = QSize( properties[ stdKey ].to_int64(), properties[ stdKey ].to_int64() );

            if ( type == "List" )
                qobject_cast<QListWidget *>( widget  )->setIconSize( iSize );

            else if ( ( name == "PowerButton" ) or ( type == "ToolButton" ) )
                qobject_cast<QToolButton*>( widget )->setIconSize( iSize );

            else if ( type == "PushButton" )
                qobject_cast<QPushButton*>( widget )->setIconSize( iSize );

            else if ( name == "Logo" )
                qobject_cast<Logo*>( widget )->setIconSize( iSize );

            else if ( type == "Combo" )
                qobject_cast<QComboBox*>( widget )->setIconSize( iSize );
        }

        else if ( key == "TextAlign" ) {
            if ( name == "UserName" and type == "Label" ) {
                if ( properties[ stdKey ].to_string() == "Center" )
                    qobject_cast<UserLabel *>( widget )->setAlignment( Qt::AlignCenter );

                else if ( properties[ stdKey ].to_string() == "Left" )
                    qobject_cast<UserLabel *>( widget )->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

                else if ( properties[ stdKey ].to_string() == "Right" )
                    qobject_cast<UserLabel *>( widget )->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
            }

            else if ( name == "SessionName" and type == "Label" ) {
                if ( properties[ stdKey ].to_string() == "Center" )
                    qobject_cast<SessionLabel *>( widget )->setAlignment( Qt::AlignCenter );

                else if ( properties[ stdKey ].to_string() == "Left" )
                    qobject_cast<SessionLabel *>( widget )->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

                else if ( properties[ stdKey ].to_string() == "Right" )
                    qobject_cast<SessionLabel *>( widget )->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
            }

            else if ( name == "QLabel" or type == "Label" ) {
                if ( properties[ stdKey ].to_string() == "Center" )
                    qobject_cast<QLabel *>( widget )->setAlignment( Qt::AlignCenter );

                else if ( properties[ stdKey ].to_string() == "Left" )
                    qobject_cast<QLabel *>( widget )->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

                else if ( properties[ stdKey ].to_string() == "Right" )
                    qobject_cast<QLabel *>( widget )->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
            }
        }

        else if ( key == "ToolTip" ) {
            widget->setToolTip( properties[ stdKey ].to_string().c_str() );
        }

        else if ( key == "NavButtons" and name == "UserName" and type == "Label" ) {
            qobject_cast<UserLabel *>( widget )->setShowNavButtons( true );
        }

        else if ( key == "NavButtons" and name == "SessionName" and type == "Label" ) {
            qobject_cast<SessionLabel *>( widget )->setShowNavButtons( true );
        }
    }
}

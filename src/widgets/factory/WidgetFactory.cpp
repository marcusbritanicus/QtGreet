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

QWidget *WidgetFactory::createWidget( QString name, QString type, Hjson::Value properties, QSize size ) {
    QWidget *w = nullptr;

    if ( name == "QWidget" ) {
        w = new QWidget();
        w->setObjectName( type );
    }

    else if ( name == "QLabel" ) {
        w = new QLabel();
        w->setObjectName( "Label" );
    }

    else if ( name == "Battery" ) {
        w = new QLabel();
        w->setObjectName( "Battery" );
        qobject_cast<QLabel *>( w )->setPixmap( QIcon::fromTheme( "battery-full-symbolic" ).pixmap( 24 ) );
        w->resize( 24, 24 );
    }

    else if ( name == "Clock" ) {
        if ( type == "Analog" ) {
            // qWarning() << "Analog clock";

            w = new AnalogClock( "#ffffff" );
            w->setObjectName( "Clock" );
        }

        else if ( type == "Digital" ) {
            // qWarning() << "Digital clock";

            w = new DigitalClock();
            w->setObjectName( "Clock" );
        }

        else {
            // qWarning() << "Simple clock";

            bool time = (type == "Time");
            bool date = (type == "Date");

            if ( not date and not time ) {
                date = true;
                time = true;
            }

            w = new SimpleClock( time, date );
            w->setObjectName( "Clock" );
        }
    }

    else if ( name == "UserIcon" ) {
        // qWarning() << "UserIcon";

        w = new UserIcon();
    }

    else if ( name == "UserName" ) {
        // qWarning() << "User Name";

        if ( type == "LineEdit" ) {
            w = new QLineEdit();
            w->setObjectName( "UserEdit" );
            qobject_cast<QLineEdit *>( w )->setPlaceholderText( "Username" );
        }

        else if ( type == "Label" ) {
            w = new UserLabel();
        }

        else if ( type == "List" ) {
            w = new UserList();
        }

        else if ( type == "Combo" ) {
            w = new UserCombo();
        }
    }

    else if ( name == "Password" ) {
        // qWarning() << "Password";

        w = new QLineEdit();
        w->setObjectName( "Password" );
        qobject_cast<QLineEdit *>( w )->setPlaceholderText( "Password" );
        qobject_cast<QLineEdit *>( w )->setEchoMode( QLineEdit::Password );
        qobject_cast<QLineEdit *>( w )->setAlignment( Qt::AlignCenter );
    }

    else if ( name == "SessionName" ) {
        // qWarning() << "Session name";

        bool custom = sett->value( "AllowCustomSessions", false ).toBool();

        if ( type == "List" ) {
            w = new SessionList( custom );
        }

        else if ( type == "Combo" ) {
            w = new SessionCombo( custom );
        }

        else if ( type == "Label" ) {
            w = new SessionLabel( custom );
        }

        else if ( type == "ToolButton" ) {
            w = new QToolButton();
            w->setObjectName( "SessionName" );
            qobject_cast<QToolButton *>( w )->setIcon( QIcon( ":/icons/session.png" ) );
        }
    }

    else if ( name == "SessionEdit" ) {
        // qWarning() << "Session Edit";

        w = new SessionEdit();
        w->setObjectName( "SessionEdit" );
        qobject_cast<QLineEdit *>( w )->setPlaceholderText( "Session command" );
    }

    else if ( name == "SessionEditButton" ) {
        // qWarning() << "SessionEdit Button";

        w = new SessionEditButton();
    }

    else if ( name == "PowerButton" ) {
        // qWarning() << "PowerButton";

        w = new PowerButton( type );
    }

    else if ( (name == "CapsLock") or (name == "NumLock") ) {
        // qWarning() << "CapsLock";

        w = new LockState( name );
    }

    else if ( (name == "UserNav") or (name == "SessionNav") ) {
        w = new NavButton( name, type );
    }

    else if ( (name == "Button") ) {
        w = new QToolButton();

        if ( type == "ToolButton" ) {
            w->setObjectName( "ToolButton" );
        }

        else if ( type == "PushButton" ) {
            w->setObjectName( "PushButton" );
        }
    }

    else if ( (name == "PushButton") ) {
        w = new QToolButton();
    }

    else if ( name == "Logo" ) {
        // qWarning() << "Logo";

        w = new Logo();
    }

    else if ( name == "LoginButton" ) {
        // qWarning() << "LoginButton";

        if ( type == "ToolButton" ) {
            w = new LoginToolButton();
        }

        else {
            w = new LoginPushButton();
        }
    }

    else if ( name == "VirtualKeyboard" ) {
        w = new QToolButton();
        w->setObjectName( "VirtualKeyboard" );
        qobject_cast<QToolButton *>( w )->setIcon( QIcon::fromTheme( "input-keyboard-virtual", QIcon::fromTheme( "input-keyboard" ) ) );
        qobject_cast<QToolButton *>( w )->setAutoRaise( true );
        w->resize( 24, 24 );
    }

    else if ( name == "Separator" ) {
        // qWarning() << "Separator";

        w = new QWidget();
        w->setStyleSheet( "background-color: gray;" );

        if ( type == "Horizontal" ) {       // To be added in a QHBoxLayout()
            w->setFixedWidth( 1 );
            w->setMinimumHeight( 1 );
        }

        else {                              // To be added in a QVBoxLayout()
            w->setFixedHeight( 1 );
            w->setMinimumWidth( 1 );
        }
    }

    else {
        w = new QWidget();
    }

    applyWidgetProperties( w, name, type, properties, size );

    return w;
}


void WidgetFactory::applyWidgetProperties( QWidget *widget, QString name, QString type, Hjson::Value properties, QSize size ) {
    for ( size_t i = 0; i < properties.size(); i++ ) {
        std::string stdKey( properties.key( i ) );
        QString     key( properties.key( i ).c_str() );

        /** Width */
        if ( key.contains( "Width" ) ) {
            int w = getWidth( properties[ stdKey ], size );

            if ( key == "MinimumWidth" ) {
                widget->setMinimumWidth( w );
            }

            else if ( key == "MaximumWidth" ) {
                widget->setMaximumWidth( w );
            }

            else {
                widget->setFixedWidth( w );
            }
        }

        /** Height */
        else if ( key.contains( "Height" ) ) {
            int h = getHeight( properties[ stdKey ], size );

            if ( key == "MinimumHeight" ) {
                widget->setMinimumHeight( h );
            }

            else if ( key == "MaximumHeight" ) {
                widget->setMaximumHeight( h );
            }

            else {
                if ( name == "UserName" and type == "Label" ) {
                    qobject_cast<UserLabel *>( widget )->setFixedHeight( h );
                }

                else if ( name == "SessionName" and type == "Label" ) {
                    qobject_cast<SessionLabel *>( widget )->setFixedHeight( h );
                }

                else {
                    widget->setFixedHeight( h );
                }
            }
        }

        /** Text */
        else if ( key == "Text" ) {
            QString text( properties[ stdKey ].to_string().c_str() );

            if ( name == "UserName" and type == "Label" ) {
                qobject_cast<UserLabel *>( widget )->setText( text );
            }

            else if ( name == "SessionName" and type == "Label" ) {
                qobject_cast<SessionLabel *>( widget )->setText( text );
            }

            else if ( type == "Label" ) {
                qobject_cast<QLabel *>( widget )->setText( text );
            }

            else if ( name == "QLabel" ) {
                qobject_cast<QLabel *>( widget )->setText( text );
            }

            else if ( type == "Combo" ) {
                qobject_cast<QComboBox *>( widget )->setCurrentText( text );
            }

            else if ( type == "PushButton" ) {
                qobject_cast<QPushButton *>( widget )->setText( text );
            }

            else if ( type == "ToolButton" ) {
                qobject_cast<QToolButton *>( widget )->setText( text );
            }
        }

        /** Icon */
        else if ( key == "Icon" ) {
            QIcon icn( properties[ stdKey ].to_string().c_str() );

            if ( type == "PushButton" ) {
                qobject_cast<QPushButton *>( widget )->setIcon( icn );
            }

            else if ( type == "ToolButton" ) {
                qobject_cast<QToolButton *>( widget )->setIcon( icn );
            }

            else if ( name == "Logo" ) {
                qobject_cast<QToolButton *>( widget )->setIcon( icn );
            }
        }

        /** Theme Icon */
        else if ( key == "ThemeIcon" ) {
            QIcon icn = QIcon::fromTheme( properties[ stdKey ].to_string().c_str() );

            if ( type == "PushButton" ) {
                qobject_cast<QPushButton *>( widget )->setIcon( icn );
            }

            else if ( (type == "ToolButton") or (name == "PowerButton") ) {
                qobject_cast<QToolButton *>( widget )->setIcon( icn );
            }
        }

        /** Font */
        else if ( key == "Font" ) {
            QString     fontFamily = sett->value( "Overrides/FontFamily" ).toString();
            QStringList fontBits   = QString( properties[ stdKey ].to_string().c_str() ).split( ", ", Qt::SkipEmptyParts );

            if ( fontFamily != "Theme" ) {
                fontBits[ 0 ] = fontFamily;
            }

            QFont font(
                fontBits[ 0 ],
                fontBits[ 1 ].toInt(),
                (fontBits[ 2 ] == "Bold" ? QFont::Bold : QFont::Normal),
                (fontBits[ 3 ] == "Italic")
            );
            widget->setFont( font );
        }

        /** FontSize - Just to scale the font size */
        else if ( key == "FontSize" ) {
            QFont font( widget->font() );

            double fs = properties[ stdKey ].to_double();

            if ( properties[ stdKey ].type() == Hjson::Type::Double ) {
                fs *= font.pointSize();
            }

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
            if ( type == "Time" ) {
                qobject_cast<SimpleClock *>( widget )->setTimeFormat( properties[ stdKey ].to_string().c_str() );
            }

            else if ( type == "Date" ) {
                qobject_cast<SimpleClock *>( widget )->setDateFormat( properties[ stdKey ].to_string().c_str() );
            }
        }

        /** Icon Size */
        else if ( key == "IconSize" ) {
            QSize iSize = QSize( properties[ stdKey ].to_int64(), properties[ stdKey ].to_int64() );

            if ( type == "List" ) {
                qobject_cast<QListWidget *>( widget )->setIconSize( iSize );
            }

            else if ( (name == "PowerButton") or (type == "ToolButton") ) {
                qobject_cast<QToolButton *>( widget )->setIconSize( iSize );
            }

            else if ( type == "PushButton" ) {
                qobject_cast<QPushButton *>( widget )->setIconSize( iSize );
            }

            else if ( name == "Logo" ) {
                qobject_cast<Logo *>( widget )->setIconSize( iSize );
            }

            else if ( type == "Combo" ) {
                qobject_cast<QComboBox *>( widget )->setIconSize( iSize );
            }
        }

        else if ( key == "ObjectName" ) {
            widget->setObjectName( properties[ stdKey ].to_string().c_str() );
        }

        else if ( key == "TextAlign" ) {
            if ( name == "UserName" and type == "Label" ) {
                if ( properties[ stdKey ].to_string() == "Center" ) {
                    qobject_cast<UserLabel *>( widget )->setAlignment( Qt::AlignCenter );
                }

                else if ( properties[ stdKey ].to_string() == "Left" ) {
                    qobject_cast<UserLabel *>( widget )->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
                }

                else if ( properties[ stdKey ].to_string() == "Right" ) {
                    qobject_cast<UserLabel *>( widget )->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
                }
            }

            else if ( name == "UserName" and type == "LineEdit" ) {
                if ( properties[ stdKey ].to_string() == "Center" ) {
                    qobject_cast<QLineEdit *>( widget )->setAlignment( Qt::AlignCenter );
                }

                else if ( properties[ stdKey ].to_string() == "Left" ) {
                    qobject_cast<QLineEdit *>( widget )->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
                }

                else if ( properties[ stdKey ].to_string() == "Right" ) {
                    qobject_cast<QLineEdit *>( widget )->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
                }
            }

            else if ( name == "SessionName" and type == "Label" ) {
                if ( properties[ stdKey ].to_string() == "Center" ) {
                    qobject_cast<SessionLabel *>( widget )->setAlignment( Qt::AlignCenter );
                }

                else if ( properties[ stdKey ].to_string() == "Left" ) {
                    qobject_cast<SessionLabel *>( widget )->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
                }

                else if ( properties[ stdKey ].to_string() == "Right" ) {
                    qobject_cast<SessionLabel *>( widget )->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
                }
            }

            else if ( name == "SessionEdit" ) {
                if ( properties[ stdKey ].to_string() == "Center" ) {
                    qobject_cast<QLineEdit *>( widget )->setAlignment( Qt::AlignCenter );
                }

                else if ( properties[ stdKey ].to_string() == "Left" ) {
                    qobject_cast<QLineEdit *>( widget )->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
                }

                else if ( properties[ stdKey ].to_string() == "Right" ) {
                    qobject_cast<QLineEdit *>( widget )->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
                }
            }

            else if ( name == "QLabel" or type == "Label" ) {
                if ( properties[ stdKey ].to_string() == "Center" ) {
                    qobject_cast<QLabel *>( widget )->setAlignment( Qt::AlignCenter );
                }

                else if ( properties[ stdKey ].to_string() == "Left" ) {
                    qobject_cast<QLabel *>( widget )->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
                }

                else if ( properties[ stdKey ].to_string() == "Right" ) {
                    qobject_cast<QLabel *>( widget )->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
                }
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

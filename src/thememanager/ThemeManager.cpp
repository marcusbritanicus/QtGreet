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

// Local Headers
#include "ThemeManager.hpp"

static QStringList themePaths = QStandardPaths::locateAll( QStandardPaths::GenericDataLocation, "qtgreet/themes", QStandardPaths::LocateDirectory );

ThemeManager::ThemeManager( QString themeName ) {
    /** Read the env variable: QTGREET_THEME_DIRS */
    for ( QString themeDir: qEnvironmentVariable( "QTGREET_THEME_DIRS" ).split( ":" ) ) {
        if ( not themePaths.contains( QDir( themeDir ).path() ) ) {
            themePaths << themeDir;
        }
    }

    /** If this theme does not exist, use the 'default' theme */
    if ( not availableThemes().contains( themeName ) ) {
        themeName = "default";
    }

    readTheme( themeName );
}


QString ThemeManager::getLayout() {
    return mLayout;
}


QString ThemeManager::getStyleSheet() {
    QFile qss( mQSS );

    qss.open( QFile::ReadOnly );
    QString ss = QString::fromLocal8Bit( qss.readAll() );

    qss.close();

    return ss;
}


QString ThemeManager::background() {
    return imageBG;
}


bool ThemeManager::isVideoBG() {
    return mIsVideoBG;
}


QString ThemeManager::video() {
    return videoBG;
}


QStringList ThemeManager::availableThemes() {
    QStringList themes;

    for ( QString path: themePaths ) {
        QDirIterator dirIt( path, QStringList(), QDir::Dirs | QDir::NoDotAndDotDot );
        while ( dirIt.hasNext() ) {
            dirIt.next();

            if ( QFile::exists( dirIt.filePath() + "/index.theme" ) ) {
                themes << dirIt.fileName();
            }
        }
    }

    return themes;
}


void ThemeManager::readTheme( QString name ) {
    QString themePath;

    for ( QString path: themePaths ) {
        if ( QFile::exists( path + "/" + name ) ) {
            themePath = path + "/" + name + "/";
            break;
        }
    }

    QSettings theme( themePath + "/index.theme", QSettings::IniFormat );

    mIsVideoBG = theme.value( "Theme/Type" ).toString() == "Video";
    mLayout    = themePath + theme.value( "Files/Layout" ).toString();
    mQSS       = themePath + theme.value( "Files/StyleSheet" ).toString();

    if ( not mIsVideoBG ) {
        imageBG = QDir( themePath ).filePath( theme.value( "Theme/Background" ).toString() );
    }

    mBaseColor = QColor( QString( "#%1" ).arg( theme.value( "Theme/BaseColor" ).toString() ) );
    mTextColor = QColor( QString( "#%1" ).arg( theme.value( "Theme/TextColor" ).toString() ) );

    /** Overrides */
    QString background = sett->value( "Overrides/Background", "Theme" ).toString();

    if ( background != "Theme" ) {
        imageBG = background;
    }

    /***/
    if ( sett->value( "videobg/File" ).toString().length() ) {
        videoBG = QDir( themePath ).filePath( sett->value( "videobg/File" ).toString() );
    }

    else {
        videoBG = QDir( themePath ).filePath( sett->value( "videobg/Playlist" ).toString() );
    }

    QString baseColor = sett->value( "Overrides/BaseColor", "Theme" ).toString();

    if ( baseColor != "Theme" ) {
        mBaseColor = QColor( QString( "#%1" ).arg( baseColor ) );
    }

    QString textColor = sett->value( "Overrides/TextColor", "Theme" ).toString();

    if ( textColor != "Theme" ) {
        mTextColor = QColor( QString( "#%1" ).arg( textColor ) );
    }
}

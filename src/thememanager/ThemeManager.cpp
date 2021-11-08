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

// Local Headers
#include "ThemeManager.hpp"

static QStringList themePaths{
    "/usr/local/share/qtgreet/themes",
    "/usr/share/qtgreet/themes",
};

ThemeManager::ThemeManager( QString themeName ) {

    /** If this theme does not exist, use the 'default' theme */
    if ( not availableThemes().contains( themeName ) )
        themeName = "default";

    readTheme( themeName );
};

QString ThemeManager::getLayout() {

    return mLayout;
};

QString ThemeManager::getStyleSheet() {

    QFile qss( mQSS );
    qss.open( QFile::ReadOnly );
    QString ss = QString::fromLocal8Bit( qss.readAll() );
    qss.close();

    return ss;
};

QString ThemeManager::background() {

    return imageBG;
};

bool ThemeManager::isVideoBG() {

    return mIsVideoBG;
};

QStringList ThemeManager::availableThemes() {

    QStringList themes;
    for( QString path: themePaths ) {
        QDirIterator dirIt( path, QStringList(), QDir::Dirs | QDir::NoDotAndDotDot );
        while( dirIt.hasNext() ) {
            dirIt.next();
            if ( QFile::exists( dirIt.filePath() + "/index.theme" ) )
                themes << dirIt.fileName();
        }
    }

    return themes;
};

void ThemeManager::readTheme( QString name ) {

    QString themePath;
    for( QString path: themePaths ) {
        if ( QFile::exists( path + "/" + name ) ) {
            themePath = path + "/" + name + "/";
            break;
        }
    }

    QSettings theme( themePath + "/index.theme", QSettings::IniFormat );

    mIsVideoBG = theme.value( "Theme/Type" ).toString() == "Video";
    mLayout = themePath + theme.value( "Files/Layout" ).toString();
    mQSS = themePath + theme.value( "Files/StyleSheet" ).toString();

    if ( not mIsVideoBG )
        imageBG = QDir( themePath ).filePath( theme.value( "Theme/Background" ).toString() );

    mBaseColor = QColor( QString( "#%1" ).arg( theme.value( "Theme/BaseColor" ).toString() ) );
    mTextColor = QColor( QString( "#%1" ).arg( theme.value( "Theme/TextColor" ).toString() ) );

    /** Overrides */
    if ( sett->value( "Overrides/Background" ) != "Theme" )
        imageBG = sett->value( "Overrides/Background" ).toString();

    if ( sett->value( "Overrides/BaseColor" ) != "Theme" )
        mBaseColor = QColor( QString( "#%1" ).arg( sett->value( "Overrides/BaseColor" ).toString() ) );

    if ( sett->value( "Overrides/TextColor" ) != "Theme" )
        mTextColor = QColor( QString( "#%1" ).arg( sett->value( "Overrides/TextColor" ).toString() ) );
};

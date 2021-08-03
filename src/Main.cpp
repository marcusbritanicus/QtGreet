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
#include "Global.hpp"
#include "QtGreet.hpp"

int main( int argc, char **argv ) {

	QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );

	QApplication app( argc, argv );

	/* Hidden test mode */
	if ( app.arguments().contains( "--test" ) ) {

		QtGreet qtgreet;
		qtgreet.showMaximized();
		return app.exec();
	}

	for( QScreen *screen: app.screens() ) {
		// Create the UI instance
		QtGreet *greet = new QtGreet();

		// Show it to get windowHandle()
		greet->show();

		// Move it @screen
		greet->windowHandle()->setScreen( screen );

		// Wait for this to happen.
		app.processEvents();

		// Set the coorect screen size
		greet->resize( screen->size() );

		// Fullscreen it
		greet->showFullScreen();
	}

	return app.exec();
};

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

		// Move it @screen - sometimes works, othertimes it does not
		greet->windowHandle()->setScreen( screen );

		// Set the correct position and size
		// This should move the window to the correct monitor
		greet->setGeometry( screen->geometry() );

		/**
		 * Since we're applying the screen geometry to the QtGreet window,
		 * we don't need to maximize it or fullscreen it.
		 * We'll just flush the event queue, before we begin with the next screen
		 */
		qApp->processEvents();
	}

	return app.exec();
};

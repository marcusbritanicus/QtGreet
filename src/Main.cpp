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

#include <qpa/qplatformnativeinterface.h>

#include "wayland-wlr-layer-shell-unstable-v1-client-protocol.h"

struct wl_display *display;
struct wl_registry *registry;
static struct zwlr_layer_shell_v1 *layer_shell;
struct zwlr_layer_surface_v1 *layer_surface;
struct wl_surface *wl_surface;

QPlatformNativeInterface *native;
QtGreet *qtgreet;

uint32_t layer = ZWLR_LAYER_SHELL_V1_LAYER_TOP;

uint32_t anchor = ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
					ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP | ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM;

static void handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
	if ( strcmp( interface, zwlr_layer_shell_v1_interface.name ) == 0 ) {
		layer_shell = (zwlr_layer_shell_v1*)wl_registry_bind( registry, name, &zwlr_layer_shell_v1_interface, 1);
	}
}

static void handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {
	// who cares
}

static const struct wl_registry_listener registry_listener = {
	.global = handle_global,
	.global_remove = handle_global_remove,
};

static void layer_surface_configure(void *data, struct zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t w, uint32_t h) {

	if ( qtgreet )
		qtgreet->resize(w, h);

	zwlr_layer_surface_v1_ack_configure( surface, serial );
}

static void layer_surface_closed( void *data, struct zwlr_layer_surface_v1 *surface ) {

	zwlr_layer_surface_v1_destroy( surface );
}

struct zwlr_layer_surface_v1_listener layer_surface_listener = {
	.configure = layer_surface_configure,
	.closed = layer_surface_closed,
};

static void show() {

	qputenv( "QT_WAYLAND_SHELL_INTEGRATION", "qtgreet" );
	qtgreet->showFullScreen();

	/* Hack to disable wayland integration */
	wl_surface = static_cast<struct wl_surface *>( native->nativeResourceForWindow( "surface", qtgreet->windowHandle() ) );
	layer_surface = zwlr_layer_shell_v1_get_layer_surface( layer_shell, wl_surface, NULL, layer, "QtGreet" );
	zwlr_layer_surface_v1_set_margin( layer_surface, 0, 0, 0, 0 );
	zwlr_layer_surface_v1_set_size( layer_surface, qtgreet->width(), qtgreet->height() );
	zwlr_layer_surface_v1_set_anchor( layer_surface, anchor );
	zwlr_layer_surface_v1_set_keyboard_interactivity( layer_surface, 1 );
	zwlr_layer_surface_v1_add_listener( layer_surface, &layer_surface_listener, layer_surface );
	wl_surface_commit( wl_surface );
	wl_display_roundtrip( display );

	/* Hack to disable wayland integration */
	qunsetenv( "QT_WAYLAND_SHELL_INTEGRATION" );
}

int main( int argc, char **argv ) {

	QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );

	QApplication app( argc, argv );

	qtgreet = new QtGreet();

	/* Hidden test mode */
	if ( app.arguments().contains( "--test" ) ) {

		qtgreet->showMaximized();
		return app.exec();
	}

	native = QGuiApplication::platformNativeInterface();
	display = ( struct wl_display * )native->nativeResourceForWindow( "display", NULL );
	registry = wl_display_get_registry( display );
	wl_registry_add_listener( registry, &registry_listener, NULL );
	wl_display_roundtrip( display );

	show();

	return app.exec();
};

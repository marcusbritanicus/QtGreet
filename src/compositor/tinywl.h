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

#pragma once

#include <wayland-util.h>
#include <wayland-server.h>
#include <wlr/util/box.h>

enum tinywl_cursor_mode {
    TINYWL_CURSOR_PASSTHROUGH,
    TINYWL_CURSOR_MOVE,
    TINYWL_CURSOR_RESIZE,
};

typedef struct tinywl_server {
    struct wl_display          *display;
    struct wlr_backend         *backend;
    struct wlr_session         *session;
    struct wlr_renderer        *renderer;
    struct wlr_allocator       *allocator;
    struct wlr_scene           *scene;

    struct wlr_xdg_shell       *xdg_shell;
    struct wl_listener         new_xdg_surface;
    struct wl_list             views;

    struct wlr_cursor          *cursor;
    struct wlr_xcursor_manager *cursor_mgr;
    struct wl_listener         cursor_motion;
    struct wl_listener         cursor_motion_absolute;
    struct wl_listener         cursor_button;
    struct wl_listener         cursor_axis;
    struct wl_listener         cursor_frame;

    struct wlr_seat            *seat;
    struct wl_listener         new_input;
    struct wl_listener         request_cursor;
    struct wl_listener         request_set_selection;
    struct wl_list             keyboards;
    enum tinywl_cursor_mode cursor_mode;
    struct tinywl_view         *grabbed_view;
    double                     grab_x, grab_y;
    struct wlr_box             grab_geobox;
    uint32_t                   resize_edges;

    struct wlr_output_layout   *output_layout;
    struct wl_list             outputs;
    struct wl_listener         new_output;
} TinyWlServer;

struct tinywl_output {
    struct wl_list       link;
    struct tinywl_server *server;
    struct wlr_output    *wlr_output;
    struct wl_listener   frame;
    struct wl_listener   destroy;
};

struct tinywl_view {
    struct wl_list          link;
    struct tinywl_server    *server;
    struct wlr_xdg_toplevel *xdg_toplevel;
    struct wlr_scene_tree   *scene_tree;
    struct wl_listener      map;
    struct wl_listener      unmap;
    struct wl_listener      destroy;
    struct wl_listener      request_move;
    struct wl_listener      request_resize;
    struct wl_listener      request_maximize;
    struct wl_listener      request_fullscreen;
    int                     x, y;
};

struct tinywl_keyboard {
    struct wl_list       link;
    struct tinywl_server *server;
    struct wlr_keyboard  *wlr_keyboard;

    struct wl_listener   modifiers;
    struct wl_listener   key;
    struct wl_listener   destroy;
};

/** Start the tinywl compositor */
int startCompositor();

/** Start the wayland event loop */
void runWlEventLoop();

/** Halt the wayland event loop */
void haltWlEventLoop();

/** Close compositor: sends the terminate signal */
void closeCompositor();

/**
 * Setup the environment to start QtGreet.
 * Set XCURSOR_THEME
 * Set XCURSOR_SIZE
 * Set QT_QPA_PLATFORM
 * Set QT_SCALE_FACTOR
 */
void setupEnv();

extern TinyWlServer *server;

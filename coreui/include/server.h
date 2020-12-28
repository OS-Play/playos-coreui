#ifndef __PLAYOS_SHELL_H_
#define __PLAYOS_SHELL_H_

#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_layer_shell_v1.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_matrix.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>
#include <xkbcommon/xkbcommon.h>

/* For brevity's sake, struct members are annotated where they are used. */
enum playos_cursor_mode {
    PLAYOS_CURSOR_PASSTHROUGH,
    PLAYOS_CURSOR_MOVE,
    PLAYOS_CURSOR_RESIZE,
};

struct playos_server {
    struct wl_display *wl_display;
    struct wlr_backend *backend;
    struct wlr_allocator *allocator;
    struct wlr_renderer *renderer;

    struct wlr_xdg_shell *xdg_shell;
    struct wl_listener new_xdg_surface;
    struct wl_list views;

    struct playos_layer_shell *layer_shell;
    struct wl_list ui_views;

    struct wlr_cursor *cursor;
    struct wlr_xcursor_manager *cursor_mgr;
    struct wl_listener cursor_motion;
    struct wl_listener cursor_motion_absolute;
    struct wl_listener cursor_button;
    struct wl_listener cursor_axis;
    struct wl_listener cursor_frame;

    struct wlr_seat *seat;
    struct wl_listener new_input;
    struct wl_listener request_cursor;
    struct wl_listener request_set_selection;
    struct wl_list keyboards;
    enum playos_cursor_mode cursor_mode;
    struct playos_view *grabbed_view;
    double grab_x, grab_y;
    struct wlr_box grab_geobox;
    uint32_t resize_edges;

    struct wlr_output_layout *output_layout;
    struct wl_list outputs;
    struct wl_listener new_output;

    const char *socket;
    const char *startup_cmd;
};

struct playos_output {
    struct wl_list link;
    struct playos_server *server;
    struct wlr_output *wlr_output;
    struct wl_listener frame;
};

struct playos_view {
    struct wl_list link;
    struct playos_server *server;

    struct wlr_xdg_surface *xdg_surface;

    struct wlr_layer_surface_v1 *layer_surface;

    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    bool mapped;
    int x, y;
};

struct playos_layer_shell {
    struct playos_server *server;

    struct wlr_layer_shell_v1 *layer_shell;

    struct wl_listener new_surface;
    struct wl_listener destroy;
};

struct playos_keyboard {
    struct wl_list link;
    struct playos_server *server;
    struct wlr_input_device *device;

    struct wl_listener modifiers;
    struct wl_listener key;
};


struct playos_server *playos_server_create();
int playos_server_init(struct playos_server *server);
void playos_server_run(struct playos_server *server);
void playos_server_destroy(struct playos_server *server);

int playos_output_init(struct playos_server *server);

int playos_views_init(struct playos_server *server);
void playos_view_focus(struct playos_view *view, struct wlr_surface *surface);

int playos_input_init(struct playos_server *server);

#endif

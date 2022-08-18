#ifndef __COREUI_SERVER_H_
#define __COREUI_SERVER_H_

#include <wayland-server-core.h>
#include <xkbcommon/xkbcommon.h>

#include <wlr/backend.h>
#include <wlr/render/allocator.h>   
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/util/log.h>

#include "shell/coreui_layer_shell.h"
#include "shell/coreui_xdg_shell.h"
#include "coreui_utils.h"

COREUI_EXTERN_BEGIN

/* For brevity's sake, struct members are annotated where they are used. */
enum coreui_cursor_mode {
    COREUI_CURSOR_PASSTHROUGH,
    COREUI_CURSOR_MOVE,
    COREUI_CURSOR_RESIZE,
};

struct coreui_server {
    struct wl_display *display;
    struct wlr_backend *backend;
    struct wlr_allocator *allocator;
    struct wlr_renderer *renderer;

    struct coreui_xdg_shell *xdg_shell;
    struct coreui_layer_shell *layer_shell;
    struct coreui_fullscreen_shell *fullscreen_shell;

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
    enum coreui_cursor_mode cursor_mode;
    struct coreui_view *grabbed_view;
    double grab_x, grab_y;
    struct wlr_box grab_geobox;
    uint32_t resize_edges;

    struct coreui_output_manager *output_manager;

    int argc;
    char **argv;

    const char *socket;
    char *startup_cmd;
};

struct coreui_server *coreui_server_create();
int coreui_server_init(struct coreui_server *server);
int coreui_server_run(struct coreui_server *server);
void coreui_server_destroy(struct coreui_server *server);

COREUI_EXTERN_END

#endif

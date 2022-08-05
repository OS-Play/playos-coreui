#ifndef __COREUI_VIEW_H__
#define __COREUI_VIEW_H__

#include <wayland-util.h>
#include "coreui_utils.h"

COREUI_EXTERN_BEGIN

enum coreui_view_type {
    COREUI_VIEW_XDG_SHELL,
    COREUI_VIEW_LAYER_SHELL,
};

struct coreui_view {
    struct wl_list link;
    struct coreui_server *server;

    enum coreui_view_type type;

    union {
        struct wlr_xdg_toplevel *xdg_toplevel;
        struct wlr_scene_layer_surface_v1 *layer_surface;
    } view;

    struct wlr_scene_tree *scene_tree;
    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
    struct wl_listener request_fullscreen;
    int x, y;
};

int coreui_view_init(struct coreui_server *server);
void coreui_view_focus(struct coreui_view *view, struct wlr_surface *surface);

COREUI_EXTERN_END

#endif

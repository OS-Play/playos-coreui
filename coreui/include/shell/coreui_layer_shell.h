#ifndef __PLAYOS_LAYER_SHELL_H__
#define __PLAYOS_LAYER_SHELL_H__

#include "coreui_utils.h"

COREUI_EXTERN_BEGIN

struct coreui_layer_shell {
    struct coreui_server *server;

    struct wlr_layer_shell_v1 *layer_shell;
    struct wlr_scene_tree *background;
    struct wlr_scene_tree *bottom;
    struct wlr_scene_tree *app;
    struct wlr_scene_tree *top;
    struct wlr_scene_tree *overlay;

    struct wl_listener new_surface;
    struct wl_listener destroy;
};

int coreui_layer_shell_init(struct coreui_server *server);

COREUI_EXTERN_END

#endif

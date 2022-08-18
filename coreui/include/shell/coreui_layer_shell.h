#ifndef __PLAYOS_LAYER_SHELL_H__
#define __PLAYOS_LAYER_SHELL_H__

#include "coreui_utils.h"

COREUI_EXTERN_BEGIN

struct coreui_layer_shell {
    struct coreui_server *server;

    struct wlr_layer_shell_v1 *layer_shell;

    struct wl_listener new_surface;
    struct wl_listener destroy;
};

int coreui_layer_shell_init(struct coreui_server *server);

COREUI_EXTERN_END

#endif

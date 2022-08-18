#ifndef __COREUI_XDG_SHELL_H__
#define __COREUI_XDG_SHELL_H__

#include "coreui_server.h"
#include "coreui_utils.h"

COREUI_EXTERN_BEGIN

struct coreui_xdg_shell {
    struct coreui_server *server;
    struct wlr_xdg_shell *xdg_shell;

    struct wl_listener new_xdg_surface;
};

int coreui_xdg_shell_init(struct coreui_server *server);

COREUI_EXTERN_END

#endif

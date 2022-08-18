#ifndef __COREUI_FULLSCREEN_SHELL_H__
#define __COREUI_FULLSCREEN_SHELL_H__

#include <wayland-server.h>


struct coreui_fullscreen_shell {
    struct coreui_server *server;

    struct wlr_fullscreen_shell_v1 *fullscreen_shell;
    struct wl_listener present_surface;
};

int coreui_fullscreen_shell_init(struct coreui_server *server);

#endif

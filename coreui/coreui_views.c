#include "coreui_server.h"
#include "coreui_view.h"
#include "include/coreui_view.h"
#include "shell/coreui_xdg_shell.h"
#include "shell/coreui_layer_shell.h"
#include "shell/coreui_fullscreen_shell.h"

#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_xdg_shell.h>

#include <assert.h>
#include <stdlib.h>


int coreui_views_init(struct coreui_server *server)
{
    coreui_xdg_shell_init(server);
    coreui_layer_shell_init(server);
    coreui_fullscreen_shell_init(server);

    return 0;
}

void coreui_view_init(struct coreui_view *view, struct coreui_server *server,
        struct wlr_surface *surface)
{
    assert(view);

    view->server = server;
    view->surface = surface;
}

void coreui_view_focus(struct coreui_view *view)
{
    if (view && view->vops && view->vops->focus) {
        view->vops->focus(view);
    }
}

void coreui_view_blur(struct coreui_view *view)
{
    if (view && view->vops && view->vops->blur) {
        view->vops->blur(view);
    }
}

void coreui_view_resize(struct coreui_view *view,
        int left, int right, int top, int bottom)
{
    if (view && view->vops && view->vops->resize) {
        view->vops->resize(view, left, right, top, bottom);
    }
}

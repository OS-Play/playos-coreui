#include "shell/coreui_fullscreen_shell.h"

#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_fullscreen_shell_v1.h>

#include "coreui_server.h"
#include "coreui_view.h"
#include "coreui_output.h"
#include "coreui_output_manager.h"

#include <stdlib.h>

struct coreui_layer_shell_view {
    struct coreui_view view;

    struct wlr_surface *fullscreen_shell_surface;
};


static void coreui_fullscreen_shell_present_surface(
            struct wl_listener *listener, void *data) {

    struct coreui_fullscreen_shell *shell =
            wl_container_of(listener, shell, present_surface);
    struct coreui_server *server = shell->server;
    struct wlr_fullscreen_shell_v1_present_surface_event *event = data;
    struct wlr_surface *surface = event->surface;
    struct coreui_output *output = NULL;

    struct coreui_layer_shell_view *view =
            (struct coreui_layer_shell_view *) calloc(1, sizeof(*view));
    if (view == NULL) {
        return;
    }

    view->fullscreen_shell_surface = surface;

    coreui_view_init(&view->view, server, surface);

    if (event->output) {
        output = coreui_output_manager_get_output(server->output_manager, event->output);
    } else {
        output = coreui_output_manager_get_active_output(view->view.server->output_manager);
    }

    coreui_output_add_view(output, &view->view, Layer_App);
}

int coreui_fullscreen_shell_init(struct coreui_server *server)
{
    struct coreui_fullscreen_shell *shell = 
            (struct coreui_fullscreen_shell *) calloc(1, sizeof(*shell));
    if (shell == NULL) {
        return -1;
    }

    server->fullscreen_shell = shell;
    shell->server = server;

    shell->fullscreen_shell = wlr_fullscreen_shell_v1_create(server->display);
    shell->present_surface.notify = coreui_fullscreen_shell_present_surface;
    wl_signal_add(&shell->fullscreen_shell->events.present_surface,
            &shell->present_surface);

    return 0;
}

#include "coreui_server.h"
#include "shell/coreui_layer_shell.h"
#include "coreui_input.h"
#include "coreui_output.h"
#include "coreui_view.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "wlr/render/wlr_renderer.h"
#include "wlr/render/allocator.h"
#include "wlr/types/wlr_scene.h"


struct coreui_server *coreui_server_create()
{
    struct coreui_server *server = malloc(sizeof(struct coreui_server));
    if (server == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    memset(server, 0, sizeof(*server));

    return server;
}

int coreui_server_init(struct coreui_server *server)
{
    server->display = wl_display_create();
    if (server->display == NULL) {
        errno = ENOMEM;
        return -1;
    }

    server->backend = wlr_backend_autocreate(server->display);
    server->renderer = wlr_renderer_autocreate(server->backend);

    wlr_renderer_init_wl_display(server->renderer, server->display);

    server->allocator = wlr_allocator_autocreate(server->backend, server->renderer);

    wlr_compositor_create(server->display, server->renderer);
    wlr_data_device_manager_create(server->display);

    if (coreui_output_init(server)) {
        return -1;
    }

    server->scene = wlr_scene_create();
    wlr_scene_attach_output_layout(server->scene, server->output_layout);

    if (coreui_view_init(server)) {
        return -1;
    }

    if (coreui_input_init(server)) {
        return -1;
    }

    /* Add a Unix socket to the Wayland display. */
    server->socket = wl_display_add_socket_auto(server->display);
    if (!server->socket) {
        wlr_backend_destroy(server->backend);
        return -1;
    }

    /* Start the backend. This will enumerate outputs and inputs, become the DRM
     * master, etc */
    if (!wlr_backend_start(server->backend)) {
        wlr_backend_destroy(server->backend);
        wl_display_destroy(server->display);
        return -1;
    }

    /* Set the WAYLAND_DISPLAY environment variable to our socket and run the
     * startup command if requested. */
    setenv("WAYLAND_DISPLAY", server->socket, true);
    if (server->startup_cmd) {
        if (fork() == 0) {
            execl("/bin/sh", "/bin/sh", "-c", server->startup_cmd, (void *)NULL);
        }
    }

    return 0;
}

int coreui_server_run(struct coreui_server *server)
{
    /* Run the Wayland event loop. This does not return until you exit the
     * compositor. Starting the backend rigged up all of the necessary event
     * loop configuration to listen to libinput events, DRM events, generate
     * frame events at the refresh rate, and so on. */
    wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s", server->socket);
    wl_display_run(server->display);

    return 0;
}

void coreui_server_destroy(struct coreui_server *server)
{
    wl_display_destroy_clients(server->display);
    wl_display_destroy(server->display);
}

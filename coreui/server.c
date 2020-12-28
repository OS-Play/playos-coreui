#include "server.h"

#include "shell/layer-shell.h"

#include <stdlib.h>
#include <unistd.h>


struct playos_server *playos_server_create()
{
    struct playos_server *server = malloc(sizeof(struct playos_server));
    if (!server)
        return NULL;

    /* The Wayland display is managed by libwayland. It handles accepting
     * clients from the Unix socket, manging Wayland globals, and so on. */
    server->wl_display = wl_display_create();
    server->socket = NULL;

    return server;
}

int playos_server_init(struct playos_server *server)
{
    /* The backend is a wlroots feature which abstracts the underlying input and
     * output hardware. The autocreate option will choose the most suitable
     * backend based on the current environment, such as opening an X11 window
     * if an X11 server is running. The NULL argument here optionally allows you
     * to pass in a custom renderer if wlr_renderer doesn't meet your needs. The
     * backend uses the renderer, for example, to fall back to software cursors
     * if the backend does not support hardware cursors (some older GPUs
     * don't). */
    server->backend = wlr_backend_autocreate(server->wl_display);

    /* If we don't provide a renderer, autocreate makes a GLES2 renderer for us.
     * The renderer is responsible for defining the various pixel formats it
     * supports for shared memory, this configures that for clients. */
    server->renderer = wlr_renderer_autocreate(server->backend);
    wlr_renderer_init_wl_display(server->renderer, server->wl_display);

    	/* Autocreates an allocator for us.
	 * The allocator is the bridge between the renderer and the backend. It
	 * handles the buffer creation, allowing wlroots to render onto the
	 * screen */
	 server->allocator = wlr_allocator_autocreate(server->backend,
		server->renderer);

    /* This creates some hands-off wlroots interfaces. The compositor is
     * necessary for clients to allocate surfaces and the data device manager
     * handles the clipboard. Each of these wlroots interfaces has room for you
     * to dig your fingers in and play with their behavior if you want. Note that
     * the clients cannot set the selection directly without compositor approval,
     * see the handling of the request_set_selection event below.*/
    wlr_compositor_create(server->wl_display, server->renderer);
    wlr_data_device_manager_create(server->wl_display);

    if (playos_output_init(server)) {
        return 1;
    }

    if (playos_views_init(server)) {
        return 1;
    }

    if (playos_input_init(server)) {
        return 1;
    }

    /* Add a Unix socket to the Wayland display. */
    server->socket = wl_display_add_socket_auto(server->wl_display);
    if (!server->socket) {
        wlr_backend_destroy(server->backend);
        return 1;
    }

    /* Start the backend. This will enumerate outputs and inputs, become the DRM
     * master, etc */
    if (!wlr_backend_start(server->backend)) {
        wlr_backend_destroy(server->backend);
        wl_display_destroy(server->wl_display);
        return 1;
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

void playos_server_run(struct playos_server *server)
{
    /* Run the Wayland event loop. This does not return until you exit the
     * compositor. Starting the backend rigged up all of the necessary event
     * loop configuration to listen to libinput events, DRM events, generate
     * frame events at the refresh rate, and so on. */
    wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s",
            server->socket);
    wl_display_run(server->wl_display);
}

void playos_server_destroy(struct playos_server *server)
{
    /* Once wl_display_run returns, we shut down the server-> */
    wl_display_destroy_clients(server->wl_display);
    wl_display_destroy(server->wl_display);

    free(server);
}

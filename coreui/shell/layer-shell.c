#include "server.h"

#include <stdlib.h>


static void layer_surface_map(struct wl_listener *listener, void *data)
{
    struct playos_view *view = wl_container_of(listener, view, map);
    view->mapped = true;
}

static void layer_surface_unmap(struct wl_listener *listener, void *data)
{
    struct playos_view *view = wl_container_of(listener, view, unmap);
    
    view->mapped = false;
}

static void layer_surface_destroy(struct wl_listener *listener, void *data)
{
    struct playos_view *view = wl_container_of(listener, view, destroy);

    wl_list_remove(&view->map.link);
    wl_list_remove(&view->unmap.link);
    wl_list_remove(&view->destroy.link);

    wl_list_remove(&view->link);

    free(view);
}


static void layer_shell_new_surface(struct wl_listener *listener, void *data)
{
    struct playos_layer_shell *layer_shell =
        wl_container_of(listener, layer_shell, new_surface);
    struct wlr_layer_surface_v1 *layer_surface = (struct wlr_layer_surface_v1 *)data;

    /* Allocate a playos_view for this surface */
    struct playos_view *view =
        (struct playos_view *)calloc(1, sizeof(struct playos_view));
    view->server = layer_shell->server;
    view->layer_surface = layer_surface;

    /* Listen to the various events it can emit */
    view->map.notify = layer_surface_map;
    wl_signal_add(&layer_surface->events.map, &view->map);
    view->unmap.notify = layer_surface_unmap;
    wl_signal_add(&layer_surface->events.unmap, &view->unmap);
    view->destroy.notify = layer_surface_destroy;
    wl_signal_add(&layer_surface->events.destroy, &view->destroy);

    // /* cotd */
    // struct wlr_xdg_toplevel *toplevel = layer_surface->toplevel;
    // view->request_move.notify = xdg_toplevel_request_move;
    // wl_signal_add(&toplevel->events.request_move, &view->request_move);
    // view->request_resize.notify = xdg_toplevel_request_resize;
    // wl_signal_add(&toplevel->events.request_resize, &view->request_resize);

    /* Add it to the list of views. */
    wl_list_insert(&layer_shell->server->ui_views, &view->link);

    if (layer_surface->output)
        wlr_layer_surface_v1_configure(layer_surface, layer_surface->output->width, layer_surface->output->height);
    else
        wlr_layer_surface_v1_configure(layer_surface, 1920, 1080);

}

static void layer_shell_closed(struct wl_listener *listener, void *data)
{

}

int playos_layer_shell_init(struct playos_server *server)
{
    struct playos_layer_shell *layer_shell = malloc(sizeof(struct playos_layer_shell));
    if (!layer_shell)
        return -1;

    server->layer_shell = layer_shell;

    layer_shell->layer_shell = wlr_layer_shell_v1_create(server->wl_display);
    layer_shell->server = server;

    layer_shell->new_surface.notify = layer_shell_new_surface;
    wl_signal_add(&layer_shell->layer_shell->events.new_surface, 
        &layer_shell->new_surface);

    layer_shell->destroy.notify = layer_shell_closed;
    wl_signal_add(&layer_shell->layer_shell->events.destroy,
        &layer_shell->destroy);

    return 0;
}
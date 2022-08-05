#include "coreui_server.h"
#include "shell/coreui_layer_shell.h"
#include "coreui_view.h"

#include <wlr/types/wlr_layer_shell_v1.h>
#include <wlr/types/wlr_scene.h>

#include <stdlib.h>


static void layer_surface_map(struct wl_listener *listener, void *data)
{
    struct coreui_view *view = wl_container_of(listener, view, map);
}

static void layer_surface_unmap(struct wl_listener *listener, void *data)
{
    struct coreui_view *view = wl_container_of(listener, view, unmap);
}

static void layer_surface_destroy(struct wl_listener *listener, void *data)
{
    struct coreui_view *view = wl_container_of(listener, view, destroy);

    wl_list_remove(&view->map.link);
    wl_list_remove(&view->unmap.link);
    wl_list_remove(&view->destroy.link);

    wl_list_remove(&view->link);

    free(view);
}


static void layer_shell_new_surface(struct wl_listener *listener, void *data)
{
    struct coreui_layer_shell *layer_shell =
        wl_container_of(listener, layer_shell, new_surface);
    struct wlr_layer_surface_v1 *layer_surface = (struct wlr_layer_surface_v1 *)data;
    struct coreui_server *server = layer_shell->server;
    struct wlr_output *output = layer_surface->output;

    /* Allocate a coreui_view for this surface */
    struct coreui_view *view = (struct coreui_view *)calloc(1, sizeof(struct coreui_view));
    view->server = server;
    view->type = COREUI_VIEW_LAYER_SHELL;

    switch (layer_surface->current.layer) {
        case ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND:
            view->view.layer_surface = 
                wlr_scene_layer_surface_v1_create(server->layer_shell->background, layer_surface);
            break;
        case ZWLR_LAYER_SHELL_V1_LAYER_BOTTOM:
            view->view.layer_surface = 
                wlr_scene_layer_surface_v1_create(server->layer_shell->bottom, layer_surface);
            break;
        case ZWLR_LAYER_SHELL_V1_LAYER_TOP:
            view->view.layer_surface = 
                wlr_scene_layer_surface_v1_create(server->layer_shell->top, layer_surface);
            break;
        case ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY:
            view->view.layer_surface = 
                wlr_scene_layer_surface_v1_create(server->layer_shell->overlay, layer_surface);
            break;
    }
    view->view.layer_surface->tree->node.data = view;

    if (output) {
        struct wlr_box full_box = {
            .x = 0,
            .y = 0,
            .width = output->width,
            .height = output->height,
        };

        struct wlr_box usable_box = {
            .x = 0,
            .y = 0,
            .width = output->width,
            .height = output->height,
        };

        wlr_scene_layer_surface_v1_configure(view->view.layer_surface, &full_box, &usable_box);
    }

    /* Listen to the various events it can emit */
    view->map.notify = layer_surface_map;
    wl_signal_add(&layer_surface->events.map, &view->map);
    view->unmap.notify = layer_surface_unmap;
    wl_signal_add(&layer_surface->events.unmap, &view->unmap);
    view->destroy.notify = layer_surface_destroy;
    wl_signal_add(&layer_surface->events.destroy, &view->destroy);

    /* Add it to the list of views. */
    wl_list_insert(&layer_shell->server->views, &view->link);

    if (output)
        wlr_layer_surface_v1_configure(layer_surface, output->width, output->height);
    else
        wlr_layer_surface_v1_configure(layer_surface, 1920, 1080);

}

static void layer_shell_closed(struct wl_listener *listener, void *data)
{

}

int coreui_layer_shell_init(struct coreui_server *server)
{
    struct coreui_layer_shell *layer_shell = calloc(1, sizeof(struct coreui_layer_shell));
    if (!layer_shell)
        return -1;

    server->layer_shell = layer_shell;

    layer_shell->layer_shell = wlr_layer_shell_v1_create(server->display);
    layer_shell->server = server;

    layer_shell->background = wlr_scene_tree_create(&server->scene->tree);
    layer_shell->bottom = wlr_scene_tree_create(&server->scene->tree);
    layer_shell->app = wlr_scene_tree_create(&server->scene->tree);
    layer_shell->top = wlr_scene_tree_create(&server->scene->tree);
    layer_shell->overlay = wlr_scene_tree_create(&server->scene->tree);

    layer_shell->new_surface.notify = layer_shell_new_surface;
    wl_signal_add(&layer_shell->layer_shell->events.new_surface, &layer_shell->new_surface);
    layer_shell->destroy.notify = layer_shell_closed;
    wl_signal_add(&layer_shell->layer_shell->events.destroy, &layer_shell->destroy);

    return 0;
}
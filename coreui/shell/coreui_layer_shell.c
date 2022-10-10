#include "coreui_server.h"
#include "shell/coreui_layer_shell.h"
#include "coreui_view.h"
#include "coreui_output_manager.h"
#include "coreui_output.h"

#include <wlr/types/wlr_layer_shell_v1.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/util/log.h>

#include <stdlib.h>
#include <stdio.h>

#define PANEL_NAMESPACE "coreui_panel"


struct coreui_layer_shell_view {
    struct coreui_view view;

    struct wlr_layer_surface_v1 *layer_surface;
    struct wlr_scene_layer_surface_v1 *scene_surface;

    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener destroy;
};

static struct wlr_scene_tree *layer_surface_tree_create(struct coreui_view *view,
        struct wlr_scene_tree *parent)
{
    struct coreui_layer_shell_view *_view =
            (struct coreui_layer_shell_view *) view;

    _view->scene_surface = wlr_scene_layer_surface_v1_create(parent,
            _view->layer_surface);

    _view->scene_surface->tree->node.data = view;

    return _view->scene_surface->tree;
}


static void layer_surface_map(struct wl_listener *listener, void *data)
{
    struct coreui_layer_shell_view *view = wl_container_of(listener, view, map);
    struct wlr_layer_surface_v1 *layer_surface = view->layer_surface;
    struct coreui_output *output = NULL;

    if (view->view.output) {
        output = view->view.output;
    } else {
        output = coreui_output_manager_get_active_output(
                view->view.server->output_manager);
    }

    enum Layer layer = Layer_Overlay;
    switch (layer_surface->current.layer) {
        case ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND:
            layer = Layer_Background;
            break;
        case ZWLR_LAYER_SHELL_V1_LAYER_BOTTOM:
            layer = Layer_Bottom;
            break;
        case ZWLR_LAYER_SHELL_V1_LAYER_TOP:
            layer = Layer_Top;
            break;
        case ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY:
            layer = Layer_Overlay;
            break;
    }

    coreui_output_add_view_with_subtree_creator(output, &view->view, layer,
            layer_surface_tree_create);

    if (output) {
        struct wlr_box full_box = {
            .x = 0,
            .y = 0,
            .width = output->wlr_output->width,
            .height = output->wlr_output->height,
        };

        struct wlr_box usable_box = {
            .x = 0,
            .y = 0,
            .width = output->wlr_output->width,
            .height = output->wlr_output->height,
        };

        wlr_scene_layer_surface_v1_configure(view->scene_surface, &full_box, &usable_box);
    }

    if (view->layer_surface->namespace && 
            strcmp(view->layer_surface->namespace, PANEL_NAMESPACE) == 0) {
        coreui_output_set_panel_height(output, view->layer_surface->current.exclusive_zone);
    }
}

static void layer_surface_unmap(struct wl_listener *listener, void *data)
{
    struct coreui_layer_shell_view *view = wl_container_of(listener, view, unmap);

    if (view->layer_surface->namespace && 
            strcmp(view->layer_surface->namespace, PANEL_NAMESPACE) == 0) {
        coreui_output_set_panel_height(view->view.output, 0);
    }
}

static void layer_surface_destroy(struct wl_listener *listener, void *data)
{
    struct coreui_layer_shell_view *view = wl_container_of(listener, view, destroy);

    wl_list_remove(&view->map.link);
    wl_list_remove(&view->unmap.link);
    wl_list_remove(&view->destroy.link);

    // wl_list_remove(&view->view.link);

    free(view);
}

static void layer_shell_new_surface(struct wl_listener *listener, void *data)
{
    struct coreui_layer_shell *layer_shell =
        wl_container_of(listener, layer_shell, new_surface);
    struct wlr_layer_surface_v1 *layer_surface = (struct wlr_layer_surface_v1 *)data;
    struct coreui_server *server = layer_shell->server;
    struct wlr_output *output = layer_surface->output;

    struct coreui_layer_shell_view *view = 
            (struct coreui_layer_shell_view *) calloc(1, sizeof(*view));
    if (view == NULL) {
        return;
    }

    coreui_view_init(&view->view, server, layer_surface->surface);

    view->view.output = coreui_output_manager_get_output(
            server->output_manager, output);
    view->layer_surface = layer_surface;

    /* Listen to the various events it can emit */
    view->map.notify = layer_surface_map;
    wl_signal_add(&layer_surface->events.map, &view->map);
    view->unmap.notify = layer_surface_unmap;
    wl_signal_add(&layer_surface->events.unmap, &view->unmap);
    view->destroy.notify = layer_surface_destroy;
    wl_signal_add(&layer_surface->events.destroy, &view->destroy);

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

    layer_shell->new_surface.notify = layer_shell_new_surface;
    wl_signal_add(&layer_shell->layer_shell->events.new_surface, &layer_shell->new_surface);
    layer_shell->destroy.notify = layer_shell_closed;
    wl_signal_add(&layer_shell->layer_shell->events.destroy, &layer_shell->destroy);

    return 0;
}
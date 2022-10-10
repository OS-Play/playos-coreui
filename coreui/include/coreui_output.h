#ifndef __COREUI_OUTPUT_H__
#define __COREUI_OUTPUT_H__

#include <wayland-util.h>
#include <wayland-server.h>
#include "coreui_utils.h"

#include "coreui_workspace.h"


COREUI_EXTERN_BEGIN

struct coreui_server;
struct coreui_view;
struct wlr_scene_tree;

struct coreui_output {
    struct wlr_output *wlr_output;

    struct coreui_server *server;
    struct coreui_workspace_manager *workspace_manager;

    struct wl_signal panel_height_changed;
    struct wl_listener frame;
    struct wl_listener destroy;
    struct wl_list link;
};

struct coreui_output *coreui_output_create(struct coreui_server *server,
        struct wlr_output *output);
struct wlr_scene *coreui_output_get_scene(struct coreui_output *output);
void coreui_output_add_view(struct coreui_output *output,
        struct coreui_view *view, enum Layer layer);
void coreui_output_add_view_with_subtree_creator(struct coreui_output *output,
        struct coreui_view *view, enum Layer layer,
        struct wlr_scene_tree *(*creator)(struct coreui_view *view, struct wlr_scene_tree *parent));
void coreui_output_set_panel_height(struct coreui_output *output, int height);
int coreui_output_get_panel_height(struct coreui_output *output);
void coreui_output_add_panel_listener(struct coreui_output *output, struct wl_listener *listener);

COREUI_EXTERN_END

#endif

#include "coreui_workspace.h"
#include "coreui_output_manager.h"
#include "coreui_output.h"
#include "coreui_server.h"
#include "coreui_view.h"

#include <wlr/types/wlr_scene.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <wayland-util.h>


struct coreui_workspace {
    int x, y;

    struct wl_list link;
};

struct coreui_workspace_manager {
    struct wl_list workspaces;

    struct coreui_output *output;
    struct coreui_workspace *active_space;

    struct wlr_scene *global_scene;

    struct wlr_scene_tree *background;
    struct wlr_scene_tree *bottom;
    struct wlr_scene_tree *app;
    struct wlr_scene_tree *top;
    struct wlr_scene_tree *overlay;

    int panel_height;
};


struct coreui_workspace_manager *coreui_workspace_manager_create(struct coreui_output *output, int workspaces)
{
    struct coreui_workspace_manager *manager = 
            (struct coreui_workspace_manager *) calloc(1, sizeof(*manager));
    if (manager == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    manager->output = output;
    wl_list_init(&manager->workspaces);

    for (int i = 0; i < workspaces; ++i) {
        if (coreui_workspace_manager_add_workspace(manager) != 0) {
            printf("workspace_manager: failed to add workspace\n");
            break;
        }
    }
    manager->global_scene = coreui_output_manager_get_scene(output->server->output_manager);

    manager->background = wlr_scene_tree_create(&manager->global_scene->tree);
    manager->bottom = wlr_scene_tree_create(&manager->global_scene->tree);
    manager->app = wlr_scene_tree_create(&manager->global_scene->tree);
    manager->top = wlr_scene_tree_create(&manager->global_scene->tree);
    manager->overlay = wlr_scene_tree_create(&manager->global_scene->tree);

    manager->active_space = wl_container_of(&manager->workspaces.prev,
            manager->active_space, link);

    return manager;
}

void coreui_workspace_manager_destroy(struct coreui_workspace_manager *m)
{
    struct coreui_workspace *p, *tmp;

    wl_list_for_each_safe(p, tmp, &m->workspaces, link) {
        coreui_workspace_manager_remove_workspace(m, p);
    }

    wlr_scene_node_destroy(&m->overlay->node);
    wlr_scene_node_destroy(&m->top->node);
    wlr_scene_node_destroy(&m->app->node);
    wlr_scene_node_destroy(&m->bottom->node);
    wlr_scene_node_destroy(&m->background->node);

    free(m);
}

int coreui_workspace_manager_add_workspace(struct coreui_workspace_manager *m)
{
     struct coreui_workspace *workspace = 
            (struct coreui_workspace *) calloc(1, sizeof(*workspace));
    if (workspace == NULL) {
        errno = ENOMEM;
        return -1;
    }

    wl_list_insert(&m->workspaces, &workspace->link);

    return 0;
}

int coreui_workspace_manager_remove_workspace(struct coreui_workspace_manager *m,
        struct coreui_workspace *workspace)
{
    wl_list_remove(&workspace->link);

    free(workspace);
}

struct coreui_workspace *coreui_workspace_manager_get_active_workspace(struct coreui_workspace_manager *m)
{
    return m->active_space;
}

void coreui_workspace_manager_add_view(struct coreui_workspace_manager *manager,
        struct coreui_view *view, enum Layer layer)
{
    coreui_workspace_manager_add_view_with_subtree_creator(manager, view, layer, NULL);
}

void coreui_workspace_manager_add_view_with_subtree_creator(struct coreui_workspace_manager *manager,
        struct coreui_view *view, enum Layer layer,
        struct wlr_scene_tree *(*creator)(struct coreui_view *view, struct wlr_scene_tree *parent))
{
    struct wlr_scene_tree *tree = coreui_workspace_get_layer_tree(manager, layer);
    if (creator) {
        view->scene_tree = creator(view, tree);
    } else {
        view->scene_tree = wlr_scene_subsurface_tree_create(tree, view->surface);
    }
    view->scene_tree->node.data = view;
}

struct wlr_scene_tree *coreui_workspace_get_layer_tree(struct coreui_workspace_manager *manager,
        enum Layer layer)
{
    switch (layer) {
    case Layer_Background:
        return manager->background;
    case Layer_Bottom:
        return manager->bottom;
    case Layer_App:
        return manager->app;
    case Layer_Top:
        return manager->top;
    case Layer_Overlay:
        return manager->overlay;
    }

    return NULL;
}

void coreui_workspace_manager_set_panel_height(
        struct coreui_workspace_manager *m,int height)
{
    assert(m);
    m->panel_height = height;
    wlr_scene_node_set_position(&m->app->node, 0, height);
}

int coreui_workspace_manager_get_panel_height(
        struct coreui_workspace_manager *manager)
{
    assert(manager);

    return manager->panel_height;
}


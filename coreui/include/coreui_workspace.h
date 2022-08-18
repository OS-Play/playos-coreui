#ifndef __COREUI_WORKSPACE_H__
#define __COREUI_WORKSPACE_H__

enum Layer {
    Layer_Background,
    Layer_Bottom,
    Layer_App,
    Layer_Top,
    Layer_Overlay,
};

struct coreui_workspace;
struct coreui_workspace_manager;
struct coreui_output;
struct coreui_view;

struct wlr_scene_tree;


struct coreui_workspace_manager *coreui_workspace_manager_create(
        struct coreui_output *output, int workspaces);
void coreui_workspace_manager_destroy(struct coreui_workspace_manager *m);

int coreui_workspace_manager_add_workspace(struct coreui_workspace_manager *m);
int coreui_workspace_manager_remove_workspace(struct coreui_workspace_manager *m,
        struct coreui_workspace *workspace);
struct coreui_workspace *coreui_workspace_manager_get_active_workspace(
        struct coreui_workspace_manager *m);
void coreui_workspace_manager_add_view(struct coreui_workspace_manager *manager,
        struct coreui_view *view, enum Layer layer);
void coreui_workspace_manager_add_view_with_subtree_creator(
        struct coreui_workspace_manager *manager,
        struct coreui_view *view, enum Layer layer,
        struct wlr_scene_tree *(*creator)(struct coreui_view *view, struct wlr_scene_tree *parent));
struct wlr_scene_tree *coreui_workspace_get_layer_tree(struct coreui_workspace_manager *manager,
        enum Layer layer);
void coreui_workspace_manager_set_panel_height(struct coreui_workspace_manager *manager, int height);
int coreui_workspace_manager_get_panel_height(struct coreui_workspace_manager *manager);


#endif

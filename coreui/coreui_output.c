#include "coreui_server.h"
#include "coreui_output.h"

#include "coreui_output_manager.h"
#include "coreui_workspace.h"
#include "coreui_view.h"

#include "wlr/render/wlr_renderer.h"
#include "wlr/types/wlr_matrix.h"
#include "wlr/types/wlr_layer_shell_v1.h"
#include <wlr/types/wlr_scene.h>

#include <assert.h>
#include <errno.h>
#include <stdlib.h>


static void output_frame(struct wl_listener *listener, void *data) {
        /* This function is called every time an output is ready to display a frame,
     * generally at the output's refresh rate (e.g. 60Hz). */
    struct coreui_output *output = wl_container_of(listener, output, frame);
    struct wlr_scene *scene = coreui_output_manager_get_scene(output->server->output_manager);

    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(
            scene, output->wlr_output);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    wlr_scene_output_commit(scene_output);
    wlr_scene_output_send_frame_done(scene_output, &now);
}

static void output_destroy(struct wl_listener *listener, void *data) {
    struct coreui_output *output = wl_container_of(listener, output, destroy);

    wl_list_remove(&output->frame.link);
    wl_list_remove(&output->destroy.link);
    wl_list_remove(&output->link);

    coreui_output_manager_remove_output(output->server->output_manager, output);

    free(output);
}

struct coreui_output *coreui_output_create(struct coreui_server *server,
        struct wlr_output *wlr_output)
{
    struct coreui_output *output = 
            (struct coreui_output *) calloc(1, sizeof(*output));
    if (output == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    output->server = server;
    output->wlr_output = wlr_output;
    output->workspace_manager = coreui_workspace_manager_create(output, 1);

    output->frame.notify = output_frame;
    wl_signal_add(&wlr_output->events.frame, &output->frame);

    output->destroy.notify = output_destroy;
    wl_signal_add(&wlr_output->events.destroy, &output->destroy);

    return output;
}

struct wlr_scene *coreui_output_get_scene(struct coreui_output *output)
{
    return coreui_output_manager_get_scene(output->server->output_manager);
}

void coreui_output_add_view(struct coreui_output *output,
        struct coreui_view *view, enum Layer layer)
{
    if (!view->output) {
        view->output = output;
    }

    coreui_workspace_manager_add_view(output->workspace_manager, view, layer);
}

void coreui_output_add_view_with_subtree_creator(struct coreui_output *output,
        struct coreui_view *view, enum Layer layer,
        struct wlr_scene_tree *(*creator)(struct coreui_view *view, struct wlr_scene_tree *parent))
{
    if (!view->output) {
        view->output = output;
    }

    coreui_workspace_manager_add_view_with_subtree_creator(output->workspace_manager,
            view, layer, creator);
}

void coreui_output_set_panel_height(struct coreui_output *output, int height)
{
    coreui_workspace_manager_set_panel_height(
            output->workspace_manager, height);
}

int coreui_output_get_panel_height(struct coreui_output *output)
{
    assert(output);
    return coreui_workspace_manager_get_panel_height(output->workspace_manager);
}

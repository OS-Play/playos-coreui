#include "coreui_output_manager.h"
#include "coreui_output.h"
#include "coreui_server.h"

#include <wayland-util.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_scene.h>

#include <assert.h>
#include <errno.h>
#include <stdlib.h>


struct coreui_output_manager {
    struct coreui_server *server;

    struct wlr_scene *scene;
    struct wlr_output_layout *output_layout;
    struct wl_list outputs;

    struct coreui_output *active_output;
    struct wl_listener new_output;
};

static void server_new_output(struct wl_listener *listener, void *data) {
    /* This event is rasied by the backend when a new output (aka a display or
     * monitor) becomes available. */
    struct coreui_output_manager *manager =
            wl_container_of(listener, manager, new_output);
    struct coreui_server *server = manager->server;
    struct wlr_output *wlr_output = data;

    /* Configures the output created by the backend to use our allocator
     * and our renderer. Must be done once, before commiting the output */
    wlr_output_init_render(wlr_output, server->allocator, server->renderer);

    /* Some backends don't have modes. DRM+KMS does, and we need to set a mode
     * before we can use the output. The mode is a tuple of (width, height,
     * refresh rate), and each monitor supports only a specific set of modes. We
     * just pick the monitor's preferred mode, a more sophisticated compositor
     * would let the user configure it. */
    if (!wl_list_empty(&wlr_output->modes)) {
        struct wlr_output_mode *mode = wlr_output_preferred_mode(wlr_output);
        wlr_output_set_mode(wlr_output, mode);
        wlr_output_enable(wlr_output, true);
        if (!wlr_output_commit(wlr_output)) {
            return;
        }
    }

    struct coreui_output *output = coreui_output_create(server, wlr_output);
    if (output == NULL) {
        return;
    }

    if (manager->active_output == NULL) {
        manager->active_output = output;
    }

    wl_list_insert(&manager->outputs, &output->link);
    wlr_output_layout_add_auto(manager->output_layout, wlr_output);
}

struct coreui_output_manager *coreui_output_manager_create(struct coreui_server *server)
{
    struct coreui_output_manager *manager =
            (struct coreui_output_manager *) calloc(1, sizeof(*manager));
    if (manager == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    manager->server = server;
    manager->scene = wlr_scene_create();
    manager->output_layout = wlr_output_layout_create();

    wlr_scene_attach_output_layout(manager->scene, manager->output_layout);

    wl_list_init(&manager->outputs);
    manager->new_output.notify = server_new_output;
    wl_signal_add(&server->backend->events.new_output, &manager->new_output);

    return manager;
}

void coreui_output_manager_destroy(struct coreui_output_manager *m)
{
    wl_list_remove(&m->new_output.link);
    wlr_output_layout_destroy(m->output_layout);
    free(m);
}

void coreui_output_manager_remove_output(struct coreui_output_manager *m, struct coreui_output *output)
{
    wl_list_remove(&output->link);
}

struct wlr_scene *coreui_output_manager_get_scene(struct coreui_output_manager *m)
{
    if (m)
        return m->scene;
}

struct coreui_output *coreui_output_manager_get_output(struct coreui_output_manager *m,
        struct wlr_output *output)
{
    struct coreui_output *out;

    wl_list_for_each(out, &m->outputs, link) {
        if (out->wlr_output == output) {
            return out;
        }
    }

    return NULL;
}

struct wlr_output_layout *coreui_output_manager_get_output_layout(
                struct coreui_output_manager *m)
{
    assert(m);
    return m->output_layout;
}

struct coreui_output *coreui_output_manager_get_active_output(struct coreui_output_manager *m)
{
    if (m) {
        return m->active_output;
    }
}

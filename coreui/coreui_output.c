#include "coreui_server.h"
#include "coreui_output.h"
#include "coreui_view.h"

#include "wlr/render/wlr_renderer.h"
#include "wlr/types/wlr_matrix.h"
#include "wlr/types/wlr_layer_shell_v1.h"
#include <wlr/types/wlr_scene.h>

#include <stdlib.h>


/* Used to move all of the data necessary to render a surface from the top-level
 * frame handler to the per-surface render function. */
struct render_data {
    struct wlr_output *output;
    struct wlr_renderer *renderer;
    struct coreui_view *view;
    struct timespec *when;
};

static void render_surface(struct wlr_surface *surface,
        int sx, int sy, void *data) {
    /* This function is called for every surface that needs to be rendered. */
    struct render_data *rdata = data;
    struct coreui_view *view = rdata->view;
    struct wlr_output *output = rdata->output;

    /* We first obtain a wlr_texture, which is a GPU resource. wlroots
     * automatically handles negotiating these with the client. The underlying
     * resource could be an opaque handle passed from the client, or the client
     * could have sent a pixel buffer which we copied to the GPU, or a few other
     * means. You don't have to worry about this, wlroots takes care of it. */
    struct wlr_texture *texture = wlr_surface_get_texture(surface);
    if (texture == NULL) {
        return;
    }

    /* The view has a position in layout coordinates. If you have two displays,
     * one next to the other, both 1080p, a view on the rightmost display might
     * have layout coordinates of 2000,100. We need to translate that to
     * output-local coordinates, or (2000 - 1920). */
    double ox = 0, oy = 0;
    wlr_output_layout_output_coords(
            view->server->output_layout, output, &ox, &oy);
    ox += view->x + sx, oy += view->y + sy;

    /* We also have to apply the scale factor for HiDPI outputs. This is only
     * part of the puzzle, TinyWL does not fully support HiDPI. */
    struct wlr_box box = {
        .x = ox * output->scale,
        .y = oy * output->scale,
        .width = surface->current.width * output->scale,
        .height = surface->current.height * output->scale,
    };

    /*
     * Those familiar with OpenGL are also familiar with the role of matricies
     * in graphics programming. We need to prepare a matrix to render the view
     * with. wlr_matrix_project_box is a helper which takes a box with a desired
     * x, y coordinates, width and height, and an output geometry, then
     * prepares an orthographic projection and multiplies the necessary
     * transforms to produce a model-view-projection matrix.
     *
     * Naturally you can do this any way you like, for example to make a 3D
     * compositor.
     */
    float matrix[9];
    enum wl_output_transform transform =
        wlr_output_transform_invert(surface->current.transform);
    wlr_matrix_project_box(matrix, &box, transform, 0,
        output->transform_matrix);

    /* This takes our matrix, the texture, and an alpha, and performs the actual
     * rendering on the GPU. */
    wlr_render_texture_with_matrix(rdata->renderer, texture, matrix, 1);

    /* This lets the client know that we've displayed that frame and it can
     * prepare another one now if it likes. */
    wlr_surface_send_frame_done(surface, rdata->when);
}

static void output_frame(struct wl_listener *listener, void *data) {
        /* This function is called every time an output is ready to display a frame,
     * generally at the output's refresh rate (e.g. 60Hz). */
    struct coreui_output *output = wl_container_of(listener, output, frame);
    struct wlr_scene *scene = output->server->scene;

    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(
        scene, output->wlr_output);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    // struct coreui_view *view;
    // wl_list_for_each_reverse(view, &output->server->ui_views, link) {
    //     struct render_data rdata = {
    //         .output = output->wlr_output,
    //         .view = view,
    //         .renderer = output->server->renderer,
    //         .when = &now,
    //     };

    //     wlr_layer_surface_v1_for_each_surface(view->layer_surface,
    //             render_surface, &rdata);
    // }

    /* Render the scene if needed and commit the output */
    wlr_scene_output_commit(scene_output);

    wlr_scene_output_send_frame_done(scene_output, &now);
}

static void output_destroy(struct wl_listener *listener, void *data) {
    struct coreui_output *output = wl_container_of(listener, output, destroy);

    wl_list_remove(&output->frame.link);
    wl_list_remove(&output->destroy.link);
    wl_list_remove(&output->link);
    free(output);
}

static void server_new_output(struct wl_listener *listener, void *data) {
    /* This event is rasied by the backend when a new output (aka a display or
     * monitor) becomes available. */
    struct coreui_server *server =
        wl_container_of(listener, server, new_output);
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

    /* Allocates and configures our state for this output */
    struct coreui_output *output =
        calloc(1, sizeof(struct coreui_output));
    output->wlr_output = wlr_output;
    output->server = server;
    /* Sets up a listener for the frame notify event. */
    output->frame.notify = output_frame;
    wl_signal_add(&wlr_output->events.frame, &output->frame);

    /* Sets up a listener for the destroy notify event. */
    output->destroy.notify = output_destroy;
    wl_signal_add(&wlr_output->events.destroy, &output->destroy);

    wl_list_insert(&server->outputs, &output->link);

    wlr_output_layout_add_auto(server->output_layout, wlr_output);
}

int coreui_output_init(struct coreui_server *server)
{
    /* Creates an output layout, which a wlroots utility for working with an
     * arrangement of screens in a physical layout. */
    server->output_layout = wlr_output_layout_create();

    /* Configure a listener to be notified when new outputs are available on the
     * backend. */
    wl_list_init(&server->outputs);
    server->new_output.notify = server_new_output;
    wl_signal_add(&server->backend->events.new_output, &server->new_output);

    return 0;
}

#include "wl_context.h"

#include "private/ouput.h"
#include <wayland-client.h>
#include <stdio.h>
#include <GLES2/gl2.h>

#include "wlr-layer-shell-unstable-v1-protocol.h"

#include "private/layer_shell_config.h"


#define ZONE "coreui"


void layer_surface_configure(void *data,
        struct zwlr_layer_surface_v1 *surface,
        uint32_t serial, uint32_t w, uint32_t h) {
    zwlr_layer_surface_v1_ack_configure(surface, serial);
}

void layer_surface_closed(void *data,
        struct zwlr_layer_surface_v1 *surface) {
    // eglDestroySurface(egl_display, egl_surface);
    // wl_egl_window_destroy(egl_window);
    // zwlr_layer_surface_v1_destroy(surface);
    // wl_surface_destroy(ctx->wl_surface);
    // run_display = false;
}

static struct zwlr_layer_surface_v1_listener layer_surface_listener = {
    .configure = layer_surface_configure,
    .closed = layer_surface_closed,
};

struct zwlr_layer_surface_v1 *layer_shell_surface_create(struct wl_surface *surface, 
        struct wl_context *ctx, struct layer_shell_config *config)
{
    struct coreui_output *output = ctx->outputs[0];

    struct zwlr_layer_surface_v1 *layer_surface = zwlr_layer_shell_v1_get_layer_surface(ctx->layer_shell,
            surface, output->wl_output, config->layer, ZONE);

    // zwlr_layer_surface_v1_set_keyboard_interactivity(
    //         ctx->layer_surface, keyboard_interactive);
    zwlr_layer_surface_v1_add_listener(layer_surface,
            &layer_surface_listener, layer_surface);

    return layer_surface;
}

void layer_shell_surface_destroy(struct zwlr_layer_surface_v1 *surface)
{
    zwlr_layer_surface_v1_destroy(surface);
}

void layer_shell_setup(struct zwlr_layer_surface_v1 *layer_surface, struct layer_shell_config *config)
{
    if (config->width != 0 && config->height) {
        zwlr_layer_surface_v1_set_size(layer_surface, config->width, config->height);
    }

    if (config->anchor != 0)
        zwlr_layer_surface_v1_set_anchor(layer_surface, config->anchor);
    
    zwlr_layer_surface_v1_set_margin(layer_surface, config->top, config->right, config->bottom, config->left);
    zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, config->zone);
}

// void layer_shell_surface_set_size(struct zwlr_layer_surface_v1 *layer_surface, uint32_t width, uint32_t height)
// {
//     zwlr_layer_surface_v1_set_size(layer_surface, width, height);
// }

// void layer_shell_surface_set_anchor(struct zwlr_layer_surface_v1 *layer_surface, uint32_t anchor)
// {
//     zwlr_layer_surface_v1_set_anchor(layer_surface, anchor);
// }

// void layer_shell_surface_set_margin(struct zwlr_layer_surface_v1 *layer_surface, int32_t top, int32_t right, int32_t bottom, int32_t left)
// {
//     zwlr_layer_surface_v1_set_margin(layer_surface, top, right, bottom, left);
// }

// void layer_shell_surface_set_exclusive_zone(struct zwlr_layer_surface_v1 *zwlr_layer_surface_v1, int32_t zone)
// {
//     zwlr_layer_surface_v1_set_exclusive_zone(zwlr_layer_surface_v1, zone);
// }

#include <stddef.h>
#include <errno.h>
#include <stdlib.h>

#include <string.h>
#include <wayland-client.h>

#include "wl_context.h"
#include "private/ouput.h"



static void wl_output_geometry_cb(void *data, struct wl_output *wl_output,
        int32_t x, int32_t y, int32_t physical_width, int32_t physical_height,
        int32_t subpixel, const char *make, const char *model, int32_t transform)
{
}

static void wl_output_mode_cb(void *data,
            struct wl_output *wl_output,
            uint32_t flags,
            int32_t width,
            int32_t height,
            int32_t refresh)
{
    struct coreui_output *output = (struct coreui_output *)(data);

    output->width = width;
    output->height = height;
}

static void wl_output_done_cb(void *data,
            struct wl_output *wl_output)
{
}

static void wl_output_scale_cb(void *data,
            struct wl_output *wl_output,
            int32_t factor)
{
}

static void wl_output_name_cb(void *data,
            struct wl_output *wl_output,
            const char *name)
{
    struct coreui_output *output = (struct coreui_output *)(data);

    strncpy(output->name, name, sizeof(output->name));
}

static void wl_output_description_cb(void *data,
            struct wl_output *wl_output,
            const char *description)
{
    struct coreui_output *output = (struct coreui_output *)(data);

    strncpy(output->description, description, sizeof(output->description));
}


static struct wl_output_listener wl_output_listener = {
    .geometry = wl_output_geometry_cb,
    .mode = wl_output_mode_cb,
    .done = wl_output_done_cb,
    .scale = wl_output_scale_cb,
    .name = wl_output_name_cb,
    .description = wl_output_description_cb
};


struct coreui_output *coreui_output_create(struct wl_context *ctx, struct wl_output *_output)
{
    struct coreui_output *output = malloc(sizeof(struct coreui_output));
    if (output == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    memset(output, 0, sizeof(*output));
    output->wl_output = _output;
    output->ctx = ctx;

    wl_output_add_listener(_output, &wl_output_listener, output);

    wl_context_addOutput(ctx, output);

    return output;
}

void coreui_output_destroy(struct coreui_output *output)
{
    wl_context_removeOutput(output->ctx, output);
    free(output);
}



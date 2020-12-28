#include "private.h"
#include "output.h"

#include <wayland-client.h>
#include <stdio.h>


static void wl_output_geometry_cb(void *data, struct wl_output *wl_output,
        int32_t x, int32_t y, int32_t physical_width, int32_t physical_height,
        int32_t subpixel, const char *make, const char *model, int32_t transform);
static void wl_output_mode_cb(void *data,
            struct wl_output *wl_output,
            uint32_t flags,
            int32_t width,
            int32_t height,
            int32_t refresh);
static void wl_output_done_cb(void *data,
            struct wl_output *wl_output);
static void wl_output_scale_cb(void *data,
            struct wl_output *wl_output,
            int32_t factor);
static void wl_output_name_cb(void *data,
            struct wl_output *wl_output,
            const char *name);
static void wl_output_description_cb(void *data,
            struct wl_output *wl_output,
            const char *description);

static struct wl_output_listener wl_output_listener = {
    .geometry = wl_output_geometry_cb,
    .mode = wl_output_mode_cb,
    .done = wl_output_done_cb,
    .scale = wl_output_scale_cb,
    .name = wl_output_name_cb,
    .description = wl_output_description_cb
};


void pos_output_set_listener(struct pos_wallpaper *ctx)
{
    wl_output_add_listener(ctx->wl_output, &wl_output_listener, ctx);
}

void wl_output_geometry_cb(void *data, struct wl_output *wl_output,
        int32_t x, int32_t y, int32_t physical_width, int32_t physical_height,
        int32_t subpixel, const char *make, const char *model, int32_t transform)
{
    // printf("wl_output_geometry_cb: %d, %d, %d, %d\n", x, y, physical_width, physical_height);
}

void wl_output_mode_cb(void *data,
            struct wl_output *wl_output,
            uint32_t flags,
            int32_t width,
            int32_t height,
            int32_t refresh)
{
    // printf("wl_output_mode_cb: %d, %d, %d, %d\n", flags, width, height, refresh);
    struct pos_wallpaper *ctx = (struct pos_wallpaper *)(data);

    ctx->width = width;
    ctx->height = height;
}

void wl_output_done_cb(void *data,
            struct wl_output *wl_output)
{
}

void wl_output_scale_cb(void *data,
            struct wl_output *wl_output,
            int32_t factor)
{

}

void wl_output_name_cb(void *data,
            struct wl_output *wl_output,
            const char *name)
{
    // printf("wl_output_name_cb: %s\n", name);
}

void wl_output_description_cb(void *data,
            struct wl_output *wl_output,
            const char *description)
{
    // printf("wl_output_description_cb: %s\n", description);
}

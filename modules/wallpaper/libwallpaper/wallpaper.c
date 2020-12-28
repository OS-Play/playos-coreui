#include "private.h"
#include "output.h"

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <sys/errno.h>

#include <EGL/egl.h>
#include <wayland-egl.h>

#include "gles.h"

#include "egl_common.h"

#define ZONE "playos_wp"


#ifdef __cplusplus
extern "C" {
#endif

static void handle_global(void *data, struct wl_registry *registry,
        uint32_t name, const char *interface, uint32_t version);
static void handle_global_remove(void *data, struct wl_registry *registry,
        uint32_t name);

static void layer_surface_configure(void *data,
        struct zwlr_layer_surface_v1 *surface,
        uint32_t serial, uint32_t w, uint32_t h);
static void layer_surface_closed(void *data,
        struct zwlr_layer_surface_v1 *surface);

static void draw_wallpaper(struct pos_wallpaper *ctx);


static const struct wl_registry_listener registry_listener = {
    .global = handle_global,
    .global_remove = handle_global_remove,
};

static struct zwlr_layer_surface_v1_listener layer_surface_listener = {
    .configure = layer_surface_configure,
    .closed = layer_surface_closed,
};


struct pos_wallpaper *pos_wallpaper_init(struct wl_display *display)
{
    struct pos_wallpaper *ctx = malloc(sizeof(struct pos_wallpaper));
    if (ctx == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    memset(ctx, 0, sizeof(*ctx));

    ctx->display = display;

    if (ctx->display == NULL) {
        ctx->display = wl_display_connect(NULL);
    }

    if (ctx->display == NULL) {
        errno = ENOENT;
        printf("Failed to connect wayland\n");
        goto error_exit;
    }

    egl_init(ctx->display);

    ctx->registry = wl_display_get_registry(ctx->display);
    if (ctx->registry == NULL) {
        errno = ENOENT;
        goto error_exit;
    }
    wl_registry_add_listener(ctx->registry, &registry_listener, ctx);
    wl_display_dispatch(ctx->display);
    wl_display_roundtrip(ctx->display);

    ctx->wl_surface = wl_compositor_create_surface(ctx->compositor);
    ctx->layer_surface = zwlr_layer_shell_v1_get_layer_surface(ctx->layer_shell,
            ctx->wl_surface, ctx->wl_output, ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND, ZONE);

    zwlr_layer_surface_v1_set_size(ctx->layer_surface, ctx->width, ctx->height);
    zwlr_layer_surface_v1_set_anchor(ctx->layer_surface, ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP);
    zwlr_layer_surface_v1_set_margin(ctx->layer_surface, 0, 0, 0, 0);
    // zwlr_layer_surface_v1_set_keyboard_interactivity(
    //         ctx->layer_surface, keyboard_interactive);
    zwlr_layer_surface_v1_add_listener(ctx->layer_surface,
            &layer_surface_listener, ctx->layer_surface);
    wl_surface_commit(ctx->wl_surface);
    wl_display_roundtrip(ctx->display);

    ctx->egl_window = wl_egl_window_create(ctx->wl_surface,  ctx->width, ctx->height);
    ctx->egl_surface = eglCreatePlatformWindowSurfaceEXT(
        egl_display, egl_config, ctx->egl_window, NULL);

    wl_display_roundtrip(ctx->display);


    egl_make_current(ctx->egl_surface, ctx->egl_surface);
    if (!gladLoadGLES2Loader((GLADloadproc)eglGetProcAddress)) {
        printf("Failed to load gles library\n");
        goto error_exit;
    }
    gles_init();

    draw_wallpaper(ctx);

    return ctx;

error_exit:
    free(ctx);
    ctx = NULL;
    return NULL;
}

void pos_wallpaper_deinit(struct pos_wallpaper *ctx)
{
    assert(ctx);
    gles_destroy();
    egl_finish();
    free(ctx);
}

int pos_wallpaper_run(struct pos_wallpaper *ctx)
{
    assert(ctx);
    return wl_display_dispatch(ctx->display) == -1;
}

void pos_wallpaper_set_mode(struct pos_wallpaper *ctx, enum pos_scale_mode mode)
{
    ctx->mode = mode;
}

void pos_wallpaper_set_image(struct pos_wallpaper *ctx, void *image, int width, int height)
{
    ctx->image = image;
    ctx->image_width = width;
    ctx->image_height = height;

    gles_set_texture(image, width, height);
    gles_get_projection(ctx->width, ctx->height, ctx->image_width, ctx->image_height, ctx->mode, ctx->projection);
}

void handle_global(void *data, struct wl_registry *registry,
        uint32_t name, const char *interface, uint32_t version)
{
    struct pos_wallpaper *ctx = to_ctx(data);

    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        ctx->compositor = wl_registry_bind(registry, name,
                &wl_compositor_interface, 1);
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        ctx->shm = wl_registry_bind(registry, name,
                &wl_shm_interface, 1);
    } else if (strcmp(interface, "wl_output") == 0) {
        // if (output != UINT32_MAX) {
            // if (!wl_output) {
                ctx->wl_output = wl_registry_bind(registry, name,
                        &wl_output_interface, 1);
                pos_output_set_listener(ctx);
            // } else {
            //     output--;
            // }
        // }
    // } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    //     seat = wl_registry_bind(registry, name,
    //             &wl_seat_interface, 1);
    //     wl_seat_add_listener(seat, &seat_listener, NULL);
    } else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
        ctx->layer_shell = wl_registry_bind(registry, name,
            &zwlr_layer_shell_v1_interface, version < 4 ? version : 4);
    // } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    //     xdg_wm_base = wl_registry_bind(
    //             registry, name, &xdg_wm_base_interface, 1);
    }
}
void handle_global_remove(void *data, struct wl_registry *registry,
        uint32_t name)
{

}

void layer_surface_configure(void *data,
        struct zwlr_layer_surface_v1 *surface,
        uint32_t serial, uint32_t w, uint32_t h) {
    struct pos_wallpaper *ctx = to_ctx(data);

    zwlr_layer_surface_v1_ack_configure(surface, serial);
}

void layer_surface_closed(void *data,
        struct zwlr_layer_surface_v1 *surface) {
    struct pos_wallpaper *ctx = to_ctx(data);

    // eglDestroySurface(egl_display, egl_surface);
    // wl_egl_window_destroy(egl_window);
    // zwlr_layer_surface_v1_destroy(surface);
    // wl_surface_destroy(ctx->wl_surface);
    // run_display = false;
}

static void surface_frame_callback(
        void *data, struct wl_callback *cb, uint32_t time) {
    wl_callback_destroy(cb);
    struct pos_wallpaper *ctx = to_ctx(data);

    draw_wallpaper(ctx);
}

static struct wl_callback_listener frame_listener = {
    .done = surface_frame_callback
};

void draw_wallpaper(struct pos_wallpaper *ctx)
 {
    egl_make_current(ctx->egl_surface, ctx->egl_surface);

    glViewport(0, 0, ctx->width, ctx->height);

    gles_draw(ctx);

    struct wl_callback *frame_callback = wl_surface_frame(ctx->wl_surface);
    wl_callback_add_listener(frame_callback, &frame_listener, ctx);

    egl_swap_buffer(ctx->egl_surface);
}

#ifdef __cplusplus
}
#endif

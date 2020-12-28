#ifndef __POS_PRIVATE_H__
#define __POS_PRIVATE_H__

#include <wayland-client.h>

#include "libwallpaper.h"

#ifndef __cplusplus
#include "wlr-layer-shell-unstable-v1-protocol.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define to_ctx(ctx) (struct pos_wallpaper *)(ctx)

struct pos_wallpaper {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_shm *shm;

    struct wl_surface *wl_surface;

    struct zwlr_layer_shell_v1 *layer_shell;
    struct zwlr_layer_surface_v1 *layer_surface;
    struct wl_output *wl_output;

    int width;
    int height;

    struct wl_egl_window *egl_window;
    struct wlr_egl_surface *egl_surface;
    float projection[16];

    void *image;
    int image_width;
    int image_height;
    enum pos_scale_mode mode;
};

#ifdef __cplusplus
}
#endif

#endif

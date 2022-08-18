#ifndef __PLAYOS_WL_CONTEXT_H__
#define __PLAYOS_WL_CONTEXT_H__

#include "ouput.h"
#include "egl_wayland.h"
#include "input.h"

#include <stdint.h>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <EGL/egl.h>
#include <xkbcommon/xkbcommon.h>

#include "xdg-shell-protocol.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*registry_callback)(struct wl_registry *registry,
        uint32_t name, const char *interface, uint32_t version);

struct wl_context {
    int wlDisplayFd;
    struct wl_display *wlDisplay;
    struct wl_registry *wlRegistry;
    struct wl_compositor *wlCompositor;
    struct wl_seat *seat;
    struct wl_shm *shm;

    struct wl_pointer *pointer;
    struct wl_keyboard *keyboard;
    struct wl_touch *touch;

    struct xkb_context *xkb_ctx;
    struct xkb_keymap *xkb_keymap;
    struct xkb_state *xkb_state;

    // Cursors
    struct wl_cursor_image *cursor_image;
    struct wl_cursor_image *popup_cursor_image;
    struct wl_surface *cursor_surface;

    struct wl_list surfaces;
    struct coreui_surface *currentSurface;
    struct coreui_surface *activeSurface;

    struct xdg_wm_base *xdg_wm_base;
    struct zwlr_layer_shell_v1 *layer_shell;

    registry_callback registry_cb;

    struct wl_list outputs;

    int winWidth;
    int winHeight;

    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;

    void *priv;
};

struct wl_context *wl_context_createWithCallback(registry_callback registry_cb);
struct wl_context *wl_context_create();
int wl_context_init(struct wl_context *ctx);
int wl_context_initWithCallback(struct wl_context *ctx, registry_callback registry_cb);
void wl_context_destroy(struct wl_context *ctx);

int wl_context_dispatch(struct wl_context *ctx);

void wl_context_addOutput(struct wl_context *ctx, struct coreui_output *output);
void wl_context_removeOutput(struct wl_context *ctx, struct coreui_output *output);
struct coreui_output *wl_context_getCurrentOutput(struct wl_context *ctx);
struct coreui_surface *wl_context_createSurface(struct wl_context *ctx);
void wl_context_destroySurface(struct wl_context *ctx, struct coreui_surface *surface);

void wl_context_setPrivate(struct wl_context *ctx, void *data);

void wl_context_dispatchInputEvent(struct wl_context *ctx, struct input_event *event);

#ifdef __cplusplus
}
#endif

#endif

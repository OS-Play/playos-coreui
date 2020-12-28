#ifndef __PLAYOS_EGL_WAYLAND_H__
#define __PLAYOS_EGL_WAYLAND_H__

#include "../wl_context.h"

#include <stdbool.h>
#include <wayland-client.h>
#include <EGL/egl.h>


#ifdef __cplusplus
extern "C" {
#endif

bool egl_wayland_init(struct wl_context *ctx);
void egl_wayland_finish(struct wl_context *ctx);

struct wl_context *egl_wayland_createResourceContext(struct wl_context *ctx);

struct wl_egl_window *egl_wayland_createWindow(struct wl_surface *surface, int width, int height);
EGLSurface egl_wayland_createEGLSurfaceFromWindow(struct wl_context *ctx, struct wl_egl_window *egl_window);

void egl_wayland_swapBuffer(struct wl_context *ctx, EGLSurface surface);
void egl_wayland_makeCurrent(struct wl_context *ctx, EGLSurface surface);
void egl_wayland_clearCurrent(struct wl_context *ctx);

#ifdef __cplusplus
}
#endif

#endif

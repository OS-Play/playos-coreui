#ifndef _EGL_COMMON_H
#define _EGL_COMMON_H
#endif

#include <stdbool.h>
#include <wayland-client.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>


extern EGLDisplay egl_display;
extern EGLConfig egl_config;
extern EGLContext egl_context;

extern PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC eglCreatePlatformWindowSurfaceEXT;

bool egl_init(struct wl_display *display);

void egl_finish(void);

void egl_make_current(EGLSurface draw, EGLSurface read);

void egl_swap_buffer(EGLSurface surface);

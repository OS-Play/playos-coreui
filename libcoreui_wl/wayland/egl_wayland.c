#include "private/egl_wayland.h"
#include "wl_context.h"

#include <sys/errno.h>
#include <wayland-egl.h>
#include <EGL/eglext.h>

#include <stdio.h>
#include <string.h>

static PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT;
static PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC eglCreatePlatformWindowSurfaceEXT;

const EGLint config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE, 1,
    EGL_GREEN_SIZE, 1,
    EGL_BLUE_SIZE, 1,
    EGL_ALPHA_SIZE, 1,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE,
};

const EGLint context_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE,
};


bool egl_wayland_init(struct wl_context *ctx)
{
    const char *client_exts_str =
        eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (client_exts_str == NULL) {
        if (eglGetError() == EGL_BAD_DISPLAY) {
            fprintf(stderr, "EGL_EXT_client_extensions not supported\n");
        } else {
            fprintf(stderr, "Failed to query EGL client extensions\n");
        }
        return false;
    }

    if (!strstr(client_exts_str, "EGL_EXT_platform_base")) {
        fprintf(stderr, "EGL_EXT_platform_base not supported\n");
        return false;
    }

    if (!strstr(client_exts_str, "EGL_EXT_platform_wayland")) {
        fprintf(stderr, "EGL_EXT_platform_wayland not supported\n");
        return false;
    }

    eglGetPlatformDisplayEXT =
        (void *)eglGetProcAddress("eglGetPlatformDisplayEXT");
    if (eglGetPlatformDisplayEXT == NULL) {
        fprintf(stderr, "Failed to get eglGetPlatformDisplayEXT\n");
        return false;
    }

    eglCreatePlatformWindowSurfaceEXT =
        (void *)eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");
    if (eglCreatePlatformWindowSurfaceEXT == NULL) {
        fprintf(stderr, "Failed to get eglCreatePlatformWindowSurfaceEXT\n");
        return false;
    }

    ctx->egl_display =
        eglGetPlatformDisplayEXT(EGL_PLATFORM_WAYLAND_EXT,
            ctx->wlDisplay, NULL);
    if (ctx->egl_display == EGL_NO_DISPLAY) {
        fprintf(stderr, "Failed to create EGL display\n");
        goto error;
    }

    if (eglInitialize(ctx->egl_display, NULL, NULL) == EGL_FALSE) {
        fprintf(stderr, "Failed to initialize EGL\n");
        goto error;
    }

    EGLint matched = 0;
    if (!eglChooseConfig(ctx->egl_display, config_attribs,
            &ctx->egl_config, 1, &matched)) {
        fprintf(stderr, "eglChooseConfig failed\n");
        goto error;
    }
    if (matched == 0) {
        fprintf(stderr, "Failed to match an EGL config\n");
        goto error;
    }

    ctx->egl_context =
        eglCreateContext(ctx->egl_display, ctx->egl_config,
            EGL_NO_CONTEXT, context_attribs);
    if (ctx->egl_context == EGL_NO_CONTEXT) {
        fprintf(stderr, "Failed to create EGL context\n");
        goto error;
    }

    return true;

error:
    eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE,
        EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (ctx->egl_display) {
        eglTerminate(ctx->egl_display);
    }
    eglReleaseThread();
    return false;
}

void egl_wayland_finish(struct wl_context *ctx)
{
    eglMakeCurrent(ctx->egl_display, EGL_NO_SURFACE,
        EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(ctx->egl_display, ctx->egl_context);
    eglTerminate(ctx->egl_display);
    eglReleaseThread();
}

struct wl_context *egl_wayland_createResourceContext(struct wl_context *ctx)
{
    struct wl_context *rctx = wl_context_create();
    if (rctx == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    rctx->egl_display = ctx->egl_display;
    rctx->egl_context = eglCreateContext(ctx->egl_display, ctx->egl_config,
            ctx->egl_context, context_attribs);

    return rctx;

error:
    return NULL;
}

struct wl_egl_window *egl_wayland_createWindow(struct wl_surface *surface, int width, int height)
{
    return wl_egl_window_create(surface, width, height);
}

EGLSurface egl_wayland_createEGLSurfaceFromWindow(struct wl_context *ctx, struct wl_egl_window *egl_window)
{
    return eglCreatePlatformWindowSurfaceEXT(
        ctx->egl_display, ctx->egl_config, egl_window, NULL);
}

void egl_wayland_swapBuffer(struct wl_context *ctx, EGLSurface egl_surface)
{
    eglSwapBuffers(ctx->egl_display, egl_surface);
}

void egl_wayland_makeCurrent(struct wl_context *ctx, EGLSurface egl_surface)
{
    eglMakeCurrent(ctx->egl_display, egl_surface, egl_surface, ctx->egl_context);
}

void egl_wayland_clearCurrent(struct wl_context *ctx)
{
    eglMakeCurrent(ctx->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

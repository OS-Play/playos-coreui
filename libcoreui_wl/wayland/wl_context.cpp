#include "wl_context.hxx"
#include "private/egl_wayland.h"
#include "private/ouput.h"
#include "wl_context.h"

#include <errno.h>
#include <sys/errno.h>


namespace playos {

WLContext::WLContext()
{
}

WLContext *WLContext::create()
{
    WLContext *ctx = new WLContext();
    if (ctx->init()) {
        delete ctx;
        return nullptr;
    }

    return ctx;
}

WLContext::~WLContext()
{
    egl_wayland_finish(ctx);
    wl_context_destroy(ctx);
}

int WLContext::init()
{
    ctx = (wl_context *)(this);
    if (ctx == NULL) {
        errno = errno;
        return -1;
    }

    if (wl_context_init(ctx) != 0) {
        return -1;
    }

    if (!egl_wayland_init(ctx)) {
        errno = ENOENT;
        return -1;
    }
    wl_context_setPrivate(ctx, this);

    return 0;
}

void *WLContext::createNativeWindow(struct wl_surface *surface, int width, int height)
{
    wl_egl_window *egl_window = egl_wayland_createWindow(surface, width, height);
    if (egl_window == nullptr) {
        errno = ENOENT;
        return nullptr;
    }
    return egl_window;
}

void *WLContext::createEGLSurface(void *nativeWindow)
{
    EGLSurface egl_surface = egl_wayland_createEGLSurfaceFromWindow(ctx, (wl_egl_window *)nativeWindow);
    if (egl_surface == EGL_NO_SURFACE) {
        errno = ENOENT;
        return EGL_NO_SURFACE;
    }

    return egl_surface;
}

WLContext *WLContext::createResourceContext()
{
    WLContext *_ctx = new WLContext();
    wl_context *rctx = egl_wayland_createResourceContext(ctx);

    _ctx->ctx = rctx;

    return _ctx;
}

struct coreui_output *WLContext::getCurrentOutput()
{
    // TODO: Fix me
    return outputs[0];
}

}
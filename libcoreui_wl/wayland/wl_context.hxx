#ifndef __PLAYOS_WL_CONTEXT_HXX__
#define __PLAYOS_WL_CONTEXT_HXX__

#include "private/egl_wayland.h"
#include "private/ouput.h"
#include "wl_context.h"
#include "coreui/wayland/surface.h"

#include <cassert>
#include <cstdio>
#include <stdint.h>
#include <stdio.h>
#include <wayland-client.h>


namespace playos {

class WLContext: public wl_context {
private:
    WLContext();

protected:
    virtual int init();

public:
    static WLContext *create();
    virtual ~WLContext();

    WLContext(WLContext const&) = delete;
    WLContext& operator=(WLContext const&) = delete;


    void *createNativeWindow(struct wl_surface *surface, int width, int height);
    void *createEGLSurface(void *nativeWindow);
    WLContext *createResourceContext();

    struct coreui_output *getCurrentOutput();

    struct wl_surface *createSurface() {
        return wl_context_createSurface(ctx);
    }

    int dispatch() {
        assert(ctx);
        return wl_context_dispatch(ctx);
    }

    int dispatchPending() {
        return wl_display_dispatch_pending(ctx->wlDisplay);
    }

    int flush() {
        return wl_display_flush(ctx->wlDisplay);
    }

    void makeCurrent(EGLSurface surface) {
        assert(ctx);
        egl_wayland_makeCurrent(ctx, surface);
    }

    void clearCurrent() {
        egl_wayland_clearCurrent(ctx);
    }

    void swapBuffer(EGLSurface surface) {
        assert(ctx);
        egl_wayland_swapBuffer(ctx, surface);
    }

    int getDisplayFd() {
        return ctx->wlDisplayFd;
    }

    void *priv() {
        return ctx->priv;
    }

    void setPriv(void *data) {
        wl_context_setPrivate(ctx, data);
    }
    
    EGLContext getEGLContext() {
        return ctx->egl_context;
    }

private:
    struct wl_context *ctx;
};

}

#endif

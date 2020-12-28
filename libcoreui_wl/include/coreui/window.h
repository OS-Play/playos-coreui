#ifndef __PLAYOS_COREUI_WINDOW_H__
#define __PLAYOS_COREUI_WINDOW_H__

#include <EGL/egl.h>
#include <stdio.h>
#include <memory>
#include <string>
#include <wayland-egl.h>

#include "../wayland/wl_context.hxx"
#include "coreui/wayland/surface.h"
#include "wayland/surface.h"

namespace playos {

class Window {
private:
    Window(std::shared_ptr<WLContext> &ctx, const char *title, int width, int height);

public:
    static Window *create(std::shared_ptr<WLContext> &ctx, const char *title, int width, int height);
    static Window *create(std::shared_ptr<WLContext> &ctx, std::shared_ptr<CoreuiSurface> &surface, const char *title, int width, int height);

    void createResourceContext() {
        m_ResourceCtx.reset(m_ctx->createResourceContext());
    }

    void makeResourceCurrent() {
        m_ResourceCtx->makeCurrent(EGL_NO_SURFACE);
    }

    void makeCurrent() {
        m_ctx->makeCurrent(egl_surface);
    }

    void clearCurrent() {
        m_ctx->clearCurrent();
    }

    void swapBuffer() {
        m_ctx->swapBuffer(egl_surface);
    }

    void *getProcAddress(const char *name) {
        return (void *)eglGetProcAddress(name);
    }

    std::shared_ptr<CoreuiSurface> surface() {
        return m_surface;
    }

    void *priv() {
        return m_priv;
    }

    void setPriv(void *data) {
        m_priv = data;
    }

    std::string title() {
        return std::string(m_title);
    }
    
    int width() {
        return m_width;
    }

    int height() {
        return m_height;
    }

private:
    std::shared_ptr<WLContext> m_ctx;
    std::unique_ptr<WLContext> m_ResourceCtx;
    std::shared_ptr<CoreuiSurface> m_surface;
    struct wl_egl_window *egl_window;
    EGLSurface egl_surface;

    void *m_priv;

    char m_title[64];
    int m_width;
    int m_height;
};

}

#endif

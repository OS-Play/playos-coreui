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
#include "window_event.h"

#define WINDOW_WIDTH_FILL_SCREEN -1
#define WINDOW_HEIGHT_FILL_SCREEN -1

namespace playos {

class Window;

class MouseListener {
public:
    virtual void onMouseMove(int x, int y) = 0;
    virtual void onMouseButton(int button, int state) = 0;
    virtual void onMouseEnter(int x, int y) = 0;
    virtual void onMouseLeave() = 0;
};

class KeyboardListener {
public:
    virtual void onKeyboardKey(uint32_t keycode, uint32_t keyval, int state, int mods) = 0;
    virtual void onKeyboardEnter() { };
    virtual void onKeyboardLeave() { };
};

class WindowEventListener {
public:
    virtual void onWindowClose() = 0;
    virtual void onWindowResize(int width, int height) = 0;
    virtual void onWindowMaximum() { };
    virtual void onWindowMinimum() { };
    virtual void onWindowShow() { };
    virtual void onWindowHidden() { };
};

class DrawEventListener {
public:
    virtual void onDraw(Window *window, uint32_t time) = 0;
};

class Window: public CoreuiSurface::EventListener {
private:
    Window(std::shared_ptr<WLContext> &ctx, const char *title, int width, int height);

public:
    static Window *create(std::shared_ptr<WLContext> &ctx, const char *title, int width, int height);
    static Window *create(std::shared_ptr<WLContext> &ctx, std::shared_ptr<CoreuiSurface> &surface, const char *title, int width, int height);

    void setMouseListener(MouseListener *listener);
    void setKeyboardListener(KeyboardListener *listener);
    void setWindowEventListener(WindowEventListener *listener);
    void setDrawEventListener(DrawEventListener *listener);

    std::string getKeyUTF8(uint32_t keycode);

    void update();

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

public:
    void onMouseEvent(struct input_event *event);
    void onKeyboardEvent(struct input_event *event);
    void onEvent(EventPtr event);

private:
    void draw(uint32_t time);

private:
    std::shared_ptr<WLContext> m_ctx;
    std::unique_ptr<WLContext> m_ResourceCtx;
    std::shared_ptr<CoreuiSurface> m_surface;
    struct wl_egl_window *egl_window;
    EGLSurface egl_surface;
    MouseListener *m_mouseListener;
    KeyboardListener *m_keyboardListener;
    WindowEventListener *m_windowEventListener;
    DrawEventListener *m_drawEventListener;

    void *m_priv;

    char m_title[64];
    int m_width;
    int m_height;
};

}

#endif

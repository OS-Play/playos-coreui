#include "coreui/window.h"
#include "coreui/wayland/surface.h"
#include "coreui/window.h"
#include "wayland/private/egl_wayland.h"
#include "wayland/wl_context.hxx"

#include <string.h>


namespace playos {

Window::Window(std::shared_ptr<WLContext> &ctx, const char *title, int width, int height):
    m_ctx(ctx), m_width(width), m_height(height)
{
    strncpy(this->m_title, title, sizeof(this->m_title));
}

Window *Window::create(std::shared_ptr<WLContext> &ctx, const char *title, int width, int height)
{
    SurfaceFactory &factory = SurfaceFactory::instance();

    auto surface = factory.createSurface(ctx, CoreuiSurface::Default);

    return create(ctx, surface, title, width, height);
}

Window *Window::create(std::shared_ptr<WLContext> &ctx, std::shared_ptr<CoreuiSurface> &surface, const char *title, int width, int height)
{
    Window *win = new Window(ctx, title, width, height);
    surface->create();
    surface->commit();

    win->m_surface = surface;
    win->egl_window = (struct wl_egl_window *)ctx->createNativeWindow(surface->wl_surface(), width, height);
    win->egl_surface = ctx->createEGLSurface(win->egl_window);

    return win;
}

}

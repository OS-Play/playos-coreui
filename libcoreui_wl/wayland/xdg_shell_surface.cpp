#include "coreui/wayland/xdg_shell_surface.h"
#include "wl_context.hxx"

#include <assert.h>


extern "C" {

static void xdg_surface_handle_configure(void *data,
        struct xdg_surface *xdg_surface, uint32_t serial) {
    xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

static void xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel,
        int32_t width, int32_t height, struct wl_array *states)
{

}
static void xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel)
{
    auto surface = (playos::XdgShellSurface *)(data);

    delete surface;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure,
    .close = xdg_toplevel_close,
};

static void xdg_popup_configure(void *data, struct xdg_popup *xdg_popup,
        int32_t x, int32_t y, int32_t width, int32_t height) {
    auto surface = (playos::XdgShellSurface *)(data);

}

static void xdg_popup_done(void *data, struct xdg_popup *xdg_popup) {
    auto surface = (playos::XdgShellSurface *)(data);
    
    delete surface;
}

static const struct xdg_popup_listener xdg_popup_listener = {
    .configure = xdg_popup_configure,
    .popup_done = xdg_popup_done,
};

}

namespace playos {

XdgShellSurface::XdgShellSurface(std::shared_ptr<WLContext> &ctx):
        CoreuiSurface(ctx), m_xdg_wm_base(ctx->xdgWmBase()),
        m_xdg_surface(nullptr), m_xdg_toplevel(nullptr), m_positioner(nullptr), m_xdg_popup(nullptr),
        role(TOP_LEVEL)
{

}

XdgShellSurface::~XdgShellSurface()
{
    if (role == TOP_LEVEL) {
        xdg_toplevel_destroy(m_xdg_toplevel);
    } else {
        xdg_positioner_destroy(m_positioner);
        xdg_popup_destroy(m_xdg_popup);
    }
    xdg_surface_destroy(m_xdg_surface);
}

void XdgShellSurface::onCreate()
{
    m_xdg_surface = xdg_wm_base_get_xdg_surface(m_xdg_wm_base, m_surface->surface);
    xdg_surface_add_listener(m_xdg_surface, &xdg_surface_listener, NULL);

    if (role == TOP_LEVEL) {
        m_xdg_toplevel = xdg_surface_get_toplevel(m_xdg_surface);
        xdg_toplevel_add_listener(m_xdg_toplevel, &xdg_toplevel_listener, NULL);
    } else {
        m_positioner = xdg_wm_base_create_positioner(m_xdg_wm_base);
        assert(m_xdg_surface && m_positioner);

        // xdg_positioner_set_size(m_positioner, popup_width, popup_height);
        // xdg_positioner_set_offset(m_positioner, 0, 0);
        // xdg_positioner_set_anchor_rect(m_positioner, cur_x, cur_y, 1, 1);
        // xdg_positioner_set_anchor(m_positioner, XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT);

        // m_xdg_popup = xdg_surface_get_popup(m_xdg_surface, NULL, m_positioner);
        // xdg_popup_grab(m_xdg_popup, seat, serial);
        xdg_popup_add_listener(m_xdg_popup, &xdg_popup_listener, NULL);
    }

}

}

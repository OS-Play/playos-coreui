#ifndef __PLAYOS_XDG_SHELL_SURFACE_H__
#define __PLAYOS_XDG_SHELL_SURFACE_H__

#include "surface.h"
#include "xdg-shell-protocol.h"

namespace playos {

class XdgShellSurface: public CoreuiSurface {
public:
    enum Role {
        TOP_LEVEL,
        POPUP,
    };
public:
    XdgShellSurface(std::shared_ptr<WLContext> &ctx);
    ~XdgShellSurface();

private:
    void onCreate();

private:
    struct xdg_wm_base *m_xdg_wm_base;
    struct xdg_surface *m_xdg_surface;
    struct xdg_toplevel *m_xdg_toplevel;
    struct xdg_positioner *m_positioner;
    struct xdg_popup *m_xdg_popup;
    Role role;
};

}

#endif

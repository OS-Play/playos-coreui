#ifndef __COREUI_WAYLAND_SURFACE_H__
#define __COREUI_WAYLAND_SURFACE_H__

#include <memory>
#include <mutex>

struct wl_surface;

namespace playos {

class WLContext;

class CoreuiSurface {
public:
    enum SurfaceRole {
        Default,
        LayerShell,
    };

protected:
    virtual void onCreate() = 0;
    virtual void setup() { };

public:
    CoreuiSurface(std::shared_ptr<WLContext> &ctx):
            m_ctx(ctx), m_wlSurface(nullptr) { }

    struct wl_surface *wl_surface() {
        return m_wlSurface;
    }

    void setWlSurface(struct wl_surface *surface) {
        m_wlSurface = surface;
    }

    void create();
    void commit();

protected:
    std::shared_ptr<WLContext> m_ctx;
    struct wl_surface *m_wlSurface;
    std::once_flag m_createFlag;
};

class SurfaceFactory {
private:
    SurfaceFactory();

public:
    static SurfaceFactory &instance();

    std::shared_ptr<CoreuiSurface> createSurface(std::shared_ptr<WLContext> ctx, CoreuiSurface::SurfaceRole role);
};

}

#endif

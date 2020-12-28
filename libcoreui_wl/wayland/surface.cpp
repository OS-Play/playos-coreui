#include "coreui/wayland/surface.h"
#include "coreui/wayland/layer_shell_surface.h"
#include "wl_context.hxx"

#include <memory>
#include <mutex>

namespace playos {

void CoreuiSurface::create()
{
    std::call_once(m_createFlag, &CoreuiSurface::onCreate, this);
}

void CoreuiSurface::commit()
{
    setup();
    wl_surface_commit(m_wlSurface);
    wl_display_roundtrip(m_ctx->wlDisplay);
}

SurfaceFactory::SurfaceFactory()
{

}


SurfaceFactory &SurfaceFactory::instance()
{
    static SurfaceFactory factory;

    return factory;
}

std::shared_ptr<CoreuiSurface> SurfaceFactory::createSurface(std::shared_ptr<WLContext> ctx, CoreuiSurface::SurfaceRole role)
{
    std::shared_ptr<CoreuiSurface> surface(nullptr);
    switch (role) {
    case CoreuiSurface::Default:

        break;
    case CoreuiSurface::LayerShell:
        surface = std::make_shared<LayerShellSurface>(ctx);
        break;
    }

    return surface;
}

}

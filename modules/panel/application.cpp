#include "application.h"

#include "coreui/wayland/surface.h"
#include "coreui/wayland/layer_shell_surface.h"

#include <cstdio>
#include <memory>

namespace playos {

PanelApplication::PanelApplication(int argc, char *argv[], const char *assetsPath, const char *icudtl):
        flutter::Application(argc, argv, assetsPath, icudtl)
{
}

PanelApplication::~PanelApplication()
{
    
}

std::shared_ptr<Window> PanelApplication::createDefaultWindow()
{
    auto ctx = context();
    struct coreui_output *output = ctx->getCurrentOutput();
    auto surface = SurfaceFactory::instance().createSurface(ctx, CoreuiSurface::LayerShell);
    auto surf = std::dynamic_pointer_cast<LayerShellSurface>(surface);

    surf->setSize(output->width, 64);

    return std::shared_ptr<Window>(createWindow(surface, "main", output->width, 64));
}

}

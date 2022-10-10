#include "application.h"

#include "coreui/wayland/surface.h"
#include "coreui/wayland/layer_shell_surface.h"

#include <cstdio>
#include <memory>

#define PANEL_HEIGHT 32

namespace playos {

PanelApplication::PanelApplication(int argc, char *argv[]):
        flutter::Application(argc, argv)
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

    surf->setSize(output->width, PANEL_HEIGHT);
    surf->setLayer(LayerShellSurface::LayerBottom);
    surf->setExclusiveZone(PANEL_HEIGHT);
    surf->setAnchor(LayerShellSurface::AnchorTop);
    surf->setNamespace("coreui_panel");

    return std::shared_ptr<Window>(createWindow(surface, "main", output->width, PANEL_HEIGHT));
}

}

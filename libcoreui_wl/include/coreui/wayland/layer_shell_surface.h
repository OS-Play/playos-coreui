#ifndef __COREUI_WAYLAND_LAYER_SHELL_SURFACE_H__
#define __COREUI_WAYLAND_LAYER_SHELL_SURFACE_H__

#include "surface.h"
#include <memory>

#include <stdint.h>



struct zwlr_layer_surface_v1;
struct layer_shell_config;


namespace playos {

class LayerShellSurface: public CoreuiSurface {
public:
    enum Layer {
        LayerBackground,
        LayerBottom,
        LayerTop,
        LayerOverlay,
    };

    enum Anchor {
        AnchorTop       = 1,
        AnchorBottom    = 2,
        AnchorLeft      = 4,
        AnchorRight     = 8,
    };

public:
    LayerShellSurface(std::shared_ptr<WLContext> &ctx);
    ~LayerShellSurface();

    void setLayer(enum Layer layer);

    void setSize(uint32_t width, uint32_t height);
    void setAnchor(enum Anchor anchor);
    void setMargin(int32_t top, int32_t right, int32_t bottom, int32_t left);
    void setExclusiveZone(int32_t zone);

    void onCreate();
    void setup();
private:
    struct layer_shell_config *m_config;

    struct zwlr_layer_surface_v1 *m_layerSurface;
};

}


#endif

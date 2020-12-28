#include "coreui/wayland/layer_shell_surface.h"
#include "wl_context.h"
#include "wl_context.hxx"
#include <memory>
#include <string.h>

#include "private/layer_shell_config.h"

extern "C" {

struct zwlr_layer_surface_v1 *layer_shell_surface_create(
        struct wl_surface *surface, struct wl_context *ctx, struct layer_shell_config *config);
void layer_shell_surface_destroy(struct zwlr_layer_surface_v1 *);
void layer_shell_setup(struct zwlr_layer_surface_v1 *layer_surface, struct layer_shell_config *config);

// void layer_shell_surface_set_size(struct zwlr_layer_surface_v1 *layer_surface, uint32_t, uint32_t);
// void layer_shell_surface_set_anchor(struct zwlr_layer_surface_v1 *layer_surface, uint32_t);
// void layer_shell_surface_set_margin(struct zwlr_layer_surface_v1 *layer_surface, int32_t, int32_t, int32_t, int32_t);
// void layer_shell_surface_set_exclusive_zone(struct zwlr_layer_surface_v1 *zwlr_layer_surface_v1, int32_t zone);

}

namespace playos {


LayerShellSurface::LayerShellSurface(std::shared_ptr<WLContext> &ctx):
        CoreuiSurface(ctx), m_config(new layer_shell_config),
        m_layerSurface(nullptr)
{
    memset(m_config, 0, sizeof(*m_config));
    m_config->layer = LayerBackground;
    m_config->anchor = AnchorTop;

    m_wlSurface = ctx->createSurface();
}

LayerShellSurface::~LayerShellSurface()
{
    if (m_layerSurface != nullptr) {
        layer_shell_surface_destroy(m_layerSurface);
    }

    delete m_config;
}

void LayerShellSurface::setLayer(enum Layer layer)
{
    m_config->layer = layer;
}

void LayerShellSurface::setSize(uint32_t width, uint32_t height)
{
    m_config->width = width;
    m_config->height = height;
}

void LayerShellSurface::setAnchor(enum Anchor anchor)
{
    m_config->anchor = anchor;
}

void LayerShellSurface::setMargin(int32_t top, int32_t right, int32_t bottom, int32_t left)
{
    m_config->top = top;
    m_config->right = right;
    m_config->bottom = bottom;
    m_config->left = left;

}

void LayerShellSurface::setExclusiveZone(int32_t zone)
{
    m_config->zone = zone;
}

void LayerShellSurface::onCreate()
{
    m_layerSurface = layer_shell_surface_create(m_wlSurface, m_ctx.get(), m_config);
}

void LayerShellSurface::setup()
{
    layer_shell_setup(m_layerSurface, m_config);
}

}

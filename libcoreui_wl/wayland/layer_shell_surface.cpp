#include "coreui/wayland/layer_shell_surface.h"
#include "wl_context.h"
#include "wl_context.hxx"
#include <memory>
#include <string.h>

#include "layer_shell_config.h"

extern "C" {
#define namespace _namespace
#include "wlr-layer-shell-unstable-v1-protocol.h"
#undef namespace
}

#define ZWLR_ANCHOR_TOP ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP | \
			ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT | \
			ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT
#define ZWLR_ANCHOR_BOTTOM ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM | \
			ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT | \
			ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT
#define ZWLR_ANCHOR_LEFT ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP | \
			ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM | \
			ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT
#define ZWLR_ANCHOR_RIGHT ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP | \
			ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT | \
			ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM

extern "C" {

struct zwlr_layer_surface_v1 *layer_shell_surface_create(
        struct coreui_surface *surface, struct wl_context *ctx, struct layer_shell_config *config);
void layer_shell_surface_destroy(struct zwlr_layer_surface_v1 *);
void layer_shell_setup(struct zwlr_layer_surface_v1 *layer_surface, struct layer_shell_config *config);

}

namespace playos {


LayerShellSurface::LayerShellSurface(std::shared_ptr<WLContext> &ctx):
        CoreuiSurface(ctx), m_config(new layer_shell_config),
        m_layerSurface(nullptr)
{
    memset(m_config, 0, sizeof(*m_config));
    m_config->layer = LayerBackground;
    setAnchor(AnchorTop);
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
    switch (anchor) {
    case AnchorTop:
        m_config->anchor = ZWLR_ANCHOR_TOP;
        break;
    case AnchorBottom:
        m_config->anchor = ZWLR_ANCHOR_BOTTOM;
        break;
    case AnchorLeft:
        m_config->anchor = ZWLR_ANCHOR_LEFT;
        break;
    case AnchorRight:
        m_config->anchor = ZWLR_ANCHOR_RIGHT;
        break;
    }
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

void LayerShellSurface::setNamespace(const char *namespace_)
{
    strncpy(m_config->namespace_, namespace_, sizeof(m_config->namespace_));
}

void LayerShellSurface::onCreate()
{
    m_layerSurface = layer_shell_surface_create(m_surface, m_ctx.get(), m_config);
}

void LayerShellSurface::setup()
{
    layer_shell_setup(m_layerSurface, m_config);
}

}

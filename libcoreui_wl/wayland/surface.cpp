#include "coreui/wayland/surface.h"
#include "coreui/wayland/layer_shell_surface.h"
#include "coreui/wayland/xdg_shell_surface.h"
#include "wl_context.hxx"

#include "coreui/window_event.h"

#include <memory>
#include <mutex>

extern "C" {

void coreui_surface_setInputListener(struct coreui_surface *surface, input_event_listener listener, void *data)
{
    surface->listener = listener;
    surface->listenerData = data;
}

static void wl_surface_enter(void *data, struct wl_surface *wl_surface,
        struct wl_output *output)
{
    playos::CoreuiSurface *surface = reinterpret_cast<playos::CoreuiSurface *>(data);

    surface->setOutput(output);
}

static void wl_surface_leave(void *data, struct wl_surface *wl_surface,
        struct wl_output *output)
{
    playos::CoreuiSurface *surface = reinterpret_cast<playos::CoreuiSurface *>(data);

    surface->setOutput(nullptr);
}

static struct wl_surface_listener wl_surface_listener {
	.enter = wl_surface_enter,
	.leave = wl_surface_leave,
};

static void surface_frame_callback(
        void *data, struct wl_callback *cb, uint32_t time);

static struct wl_callback_listener frame_listener = {
    .done = surface_frame_callback
};

void surface_frame_callback(void *data, struct wl_callback *cb, uint32_t time) {
    playos::CoreuiSurface *surface = reinterpret_cast<playos::CoreuiSurface *>(data);
    surface->resetNewFrameLisener();

    auto event = playos::Event::create<playos::FrameEvent>(playos::WINDOW_FRAME, time);
    surface->dispatchEvent(event);
}

}


namespace playos {


CoreuiSurface::CoreuiSurface(std::shared_ptr<WLContext> &ctx):
    m_ctx(ctx), m_surface(nullptr), m_eventListener(nullptr)
{
    m_surface = ctx->createSurface();
    wl_surface_add_listener(m_surface->surface, &wl_surface_listener, this);
    resetNewFrameLisener();
}

CoreuiSurface::~CoreuiSurface()
{
    wl_surface_destroy(m_surface->surface);
}

void CoreuiSurface::setInputListener(input_event_listener listener, void *data)
{
    coreui_surface_setInputListener(m_surface, listener, data);
}

void CoreuiSurface::setEventListener(EventListener *listener)
{
    m_eventListener = listener;
}

void CoreuiSurface::setOutput(struct wl_output *output)
{
    if (output == nullptr)
        m_surface->output = nullptr;

    struct coreui_output *out;
    wl_list_for_each(out, &m_ctx->outputs, link) {
        if (out->wl_output == output) {
            m_surface->output = out;
            break;
        }
    }
}

void CoreuiSurface::dispatchEvent(EventPtr event)
{
    if (m_eventListener) {
        m_eventListener->onEvent(event);
    }
}

void CoreuiSurface::resetNewFrameLisener()
{
    if (m_surface->frameCallback != nullptr)
        wl_callback_destroy(m_surface->frameCallback);

    m_surface->frameCallback = wl_surface_frame(m_surface->surface);
    wl_callback_add_listener(m_surface->frameCallback, &frame_listener, this);
}

void CoreuiSurface::damage()
{
    // wl_surface_attach(m_surface->surface, nullptr,
	// 		  0, 0);
    wl_surface_damage(m_surface->surface, 0, 0,
            INT32_MAX, INT32_MAX);
    wl_surface_commit(m_surface->surface);
}

void CoreuiSurface::create()
{
    std::call_once(m_createFlag, &CoreuiSurface::onCreate, this);
}

void CoreuiSurface::commit()
{
    setup();
    wl_surface_commit(m_surface->surface);
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
    case CoreuiSurface::XdgShell:
        surface = std::make_shared<XdgShellSurface>(ctx);
        break;
    case CoreuiSurface::LayerShell:
        surface = std::make_shared<LayerShellSurface>(ctx);
        break;
    }

    return surface;
}

}

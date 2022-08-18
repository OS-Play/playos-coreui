#ifndef __COREUI_WAYLAND_SURFACE_H__
#define __COREUI_WAYLAND_SURFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <wayland-util.h>

#include "../../../wayland/input.h"
#include "../../../wayland/ouput.h"


struct coreui_surface {
    struct wl_list link;

    struct wl_surface *surface;
    input_event_listener listener;
    void *listenerData;
    struct wl_callback *frameCallback;
    struct coreui_output *output;
};

void coreui_surface_setInputListener(struct coreui_surface *surface, input_event_listener listener, void *data);

#ifdef __cplusplus
}

#include "../window_event.h"

#include <memory>
#include <mutex>

struct wl_surface;
struct wl_output;

namespace playos {

class WLContext;

class CoreuiSurface {
public:
    enum SurfaceRole {
        Default,
        LayerShell,
        XdgShell,
    };

    class EventListener {
    public:
        virtual void onEvent(EventPtr event) = 0;
    };

protected:
    virtual void onCreate() = 0;
    virtual void setup() { };

public:
    CoreuiSurface(std::shared_ptr<WLContext> &ctx);
    virtual ~CoreuiSurface();

    struct wl_surface *wl_surface() {
        return m_surface->surface;
    }

    struct coreui_surface *surface() {
        return m_surface;
    }

    void setInputListener(input_event_listener listener, void *data);
    void setEventListener(EventListener *listener);
    void setOutput(struct wl_output *output);

    void dispatchEvent(EventPtr event);
    void resetNewFrameLisener();
    void damage();

    void create();
    void commit();

protected:
    std::shared_ptr<WLContext> m_ctx;
    struct coreui_surface *m_surface;
    std::once_flag m_createFlag;
    EventListener *m_eventListener;
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

#endif

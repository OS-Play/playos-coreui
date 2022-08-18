#include "wl_context.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>

#include <wayland-client.h>

#include "input.h"
#include "registry.h"
#include "egl_wayland.h"

#include "coreui/wayland/surface.h"


struct wl_context *wl_context_create()
{
    struct wl_context *ctx = malloc(sizeof(struct wl_context));
    if (ctx == NULL) {
        return NULL;
    }

    memset(ctx, 0, sizeof(*ctx));

    return ctx;
}

int wl_context_init(struct wl_context *ctx)
{
    return wl_context_initWithCallback(ctx, NULL);
}

int wl_context_initWithCallback(struct wl_context *ctx, registry_callback registry_cb)
{
    ctx->wlDisplay = wl_display_connect(NULL);
    if (ctx->wlDisplay == NULL) {
        errno = ENOENT;
        printf("Failed to connect wayland\n");
        goto error_exit;
    }
    ctx->wlDisplayFd = wl_display_get_fd(ctx->wlDisplay);

    ctx->wlRegistry = wl_display_get_registry(ctx->wlDisplay);
    if (ctx->wlRegistry == NULL) {
        errno = ENOENT;
        goto error_exit;
    }

    wl_list_init(&ctx->outputs);
    wl_list_init(&ctx->surfaces);
    ctx->registry_cb = registry_cb;
    registry_setListener(ctx->wlRegistry, ctx);

    wl_display_dispatch(ctx->wlDisplay);
    wl_display_roundtrip(ctx->wlDisplay);

    input_pointer_init(ctx);
    
    ctx->xkb_ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    assert(ctx->xkb_ctx);

    return 0;

error_exit:
    return -1;
}

void wl_context_destroy(struct wl_context *ctx)
{
    free(ctx);
}

void wl_context_setPrivate(struct wl_context *ctx, void *data)
{
    ctx->priv = data;
}

int wl_context_dispatch(struct wl_context *ctx)
{
    return wl_display_dispatch(ctx->wlDisplay);
}

void wl_context_addOutput(struct wl_context *ctx, struct coreui_output *output)
{
    wl_list_insert(&ctx->outputs, &output->link);
}

void wl_context_removeOutput(struct wl_context *ctx, struct coreui_output *output)
{
    wl_list_remove(&output->link);
}

struct coreui_output *wl_context_getCurrentOutput(struct wl_context *ctx)
{
    struct coreui_output *output = NULL;
    // TODO: Fix me
    if (wl_list_empty(&ctx->outputs))
        return NULL;

    output = wl_container_of(ctx->outputs.next, output, link);
    return output;
}

struct coreui_surface *wl_context_createSurface(struct wl_context *ctx)
{
    struct wl_surface *surface = wl_compositor_create_surface(ctx->wlCompositor);

    struct coreui_surface *_surf = (struct coreui_surface *)calloc(1, sizeof(*_surf));
    if (_surf == NULL) {
        wl_surface_destroy(surface);
        return NULL;
    }

    _surf->surface = surface;

    wl_list_insert(&ctx->surfaces, &_surf->link);

    return _surf;
}

void wl_context_destroySurface(struct wl_context *ctx, struct coreui_surface *surface)
{
    wl_list_remove(&surface->link);
    wl_surface_destroy(surface->surface);
    free(surface);
}

void wl_context_dispatchInputEvent(struct wl_context *ctx, struct input_event *event)
{
    struct coreui_surface *surf;

    if (event->type == EVENT_MOUSE && event->mouse.type == MOUSE_ENTER) {
        wl_list_for_each(surf, &ctx->surfaces, link) {
            if (surf->surface == event->surface) {
                ctx->currentSurface = surf;
                break;
            }
        }
    }

    surf = ctx->currentSurface;
    if (surf && surf->listener)
        surf->listener(event, surf->listenerData);
}

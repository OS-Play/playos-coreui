#include "wl_context.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <wayland-client.h>

#include "private/input.h"
#include "private/registry.h"
#include "private/egl_wayland.h"


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

    ctx->registry_cb = registry_cb;
    registry_setListener(ctx->wlRegistry, ctx);

    wl_display_dispatch(ctx->wlDisplay);
    wl_display_roundtrip(ctx->wlDisplay);

    input_pointer_init(ctx);

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
    for (int i = 0; i < sizeof(ctx->outputs); ++i) {
        if (ctx->outputs[i] == NULL) {
            ctx->outputs[i] = output;
            return;
        }
    }
}

void wl_context_removeOutput(struct wl_context *ctx, struct coreui_output *output)
{
    for (int i = 0; i < sizeof(ctx->outputs); ++i) {
        if (ctx->outputs[i] == output) {
            ctx->outputs[i] = NULL;
            return;
        }
    }
}

struct wl_surface *wl_context_createSurface(struct wl_context *ctx)
{
    return wl_compositor_create_surface(ctx->wlCompositor);
}

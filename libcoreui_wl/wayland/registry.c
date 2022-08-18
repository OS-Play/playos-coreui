#include "wl_context.h"

#include <stdint.h>
#include <string.h>

#include "input.h"

#include "wlr-layer-shell-unstable-v1-protocol.h"
#include "xdg-shell-protocol.h"


static void handle_global(void *data, struct wl_registry *registry,
        uint32_t name, const char *interface, uint32_t version)
{
    struct wl_context *ctx = data;

    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        ctx->wlCompositor = wl_registry_bind(registry, name,
                &wl_compositor_interface, 1);
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        ctx->shm = wl_registry_bind(registry, name,
                &wl_shm_interface, 1);
    } else if (strcmp(interface, "wl_output") == 0) {
        coreui_output_create(ctx, wl_registry_bind(registry, name,
                &wl_output_interface, 1));
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        ctx->seat = wl_registry_bind(registry, name,
                &wl_seat_interface, 1);
        input_add_listener(ctx);
    } else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
        ctx->layer_shell = wl_registry_bind(registry, name,
            &zwlr_layer_shell_v1_interface, version < 4 ? version : 4);
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        ctx->xdg_wm_base = wl_registry_bind(
                registry, name, &xdg_wm_base_interface, 1);
    }

    if (ctx->registry_cb) {
        ctx->registry_cb(registry, name, interface, version);
    }
}

static void handle_global_remove(void *data, struct wl_registry *registry,
        uint32_t name)
{

}


static const struct wl_registry_listener registry_listener = {
    .global = handle_global,
    .global_remove = handle_global_remove,
};

void registry_setListener(struct wl_registry *registry, struct wl_context *ctx)
{
    wl_registry_add_listener(registry, &registry_listener, ctx);
}

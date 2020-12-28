#ifndef __PLAYOS_REGISTRY_H__
#define __PLAYOS_REGISTRY_H__

#ifdef __cplusplus
extern "C" {
#endif

void registry_setListener(struct wl_registry *registry, struct wl_context *ctx);

#ifdef __cplusplus
}
#endif

#endif

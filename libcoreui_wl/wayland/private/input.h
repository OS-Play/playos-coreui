#ifndef __PLAYOS_SEAT_H__
#define __PLAYOS_SEAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../wl_context.h"


void input_pointer_init(struct wl_context *ctx);

void input_add_listener(struct wl_context *ctx);

#ifdef __cplusplus
}
#endif

#endif

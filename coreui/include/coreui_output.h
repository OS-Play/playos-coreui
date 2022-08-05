#ifndef __COREUI_OUTPUT_H__
#define __COREUI_OUTPUT_H__

#include <wayland-util.h>
#include "coreui_utils.h"

COREUI_EXTERN_BEGIN

struct coreui_output {
    struct wl_list link;
    struct coreui_server *server;
    struct wlr_output *wlr_output;
    struct wl_listener frame;
    struct wl_listener destroy;
};

int coreui_output_init(struct coreui_server *server);

COREUI_EXTERN_END

#endif

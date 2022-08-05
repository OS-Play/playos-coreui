#ifndef __COREUI_KEYBOARD_H__
#define __COREUI_KEYBOARD_H__

#include <wayland-util.h>

#include "coreui_server.h"
#include "coreui_utils.h"

COREUI_EXTERN_BEGIN

struct coreui_keyboard {
    struct wl_list link;
    struct coreui_server *server;
    struct wlr_keyboard *wlr_keyboard;

    struct wl_listener modifiers;
    struct wl_listener key;
    struct wl_listener destroy;
};

void coreui_keyboard_new(struct coreui_server *server, struct wlr_input_device *device);

COREUI_EXTERN_END

#endif

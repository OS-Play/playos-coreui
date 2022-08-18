#ifndef __PLAYOS_INPUT_H__
#define __PLAYOS_INPUT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct wl_context;
struct input_event;

typedef void (*input_event_listener)(struct input_event *event, void *data);

enum input_event_key_state {
    KEY_DOWN = 1,
    KEY_UP = 0,
};

enum input_event_type {
    EVENT_MOUSE,
    EVENT_KEYBOARD,
    EVENT_TOUCH,
};

enum input_event_mouse_type {
    MOUSE_MOVE,
    MOUSE_BUTTON,
    MOUSE_ENTER,
    MOUSE_LEAVE,
};

struct input_event_mouse {
    enum input_event_mouse_type type;

    int x, y;

    int button;
    enum input_event_key_state state;
};

enum input_event_keyboard_type {
    KEYBOARD_ENTER,
    KEYBOARD_LEAVE,
    KEYBOARD_KEY,
};

struct input_event_keyboard {
    enum input_event_keyboard_type type;

    uint32_t keycode;
    uint32_t keyval;
    uint32_t mods;
    enum input_event_key_state state;
};

struct input_event {
    struct wl_surface *surface;
    enum input_event_type type;

    union {
        struct input_event_mouse mouse;
        struct input_event_keyboard keyboard;
    };
};

void input_pointer_init(struct wl_context *ctx);

void input_add_listener(struct wl_context *ctx);

#ifdef __cplusplus
}
#endif

#endif

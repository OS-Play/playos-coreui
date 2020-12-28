#include "private/input.h"

#include "wl_context.h"

#include <assert.h>
#include <stdio.h>



static void wl_pointer_enter(void *data, struct wl_pointer *wl_pointer,
        uint32_t serial, struct wl_surface *surface,
        wl_fixed_t surface_x, wl_fixed_t surface_y) {
    struct wl_cursor_image *image;
    struct wl_context *ctx = data;

printf("wl_pointer_enter: %p\n", wl_pointer);
    // if (surface == ctx->popup_wl_surface) {
    //     image = popup_cursor_image;
    // } else {
        image = ctx->cursor_image;
    // }
    wl_surface_attach(ctx->cursor_surface,
        wl_cursor_image_get_buffer(image), 0, 0);
    wl_surface_damage(ctx->cursor_surface, 1, 0,
        image->width, image->height);
    wl_surface_commit(ctx->cursor_surface);
    wl_pointer_set_cursor(wl_pointer, serial, ctx->cursor_surface,
        image->hotspot_x, image->hotspot_y);
    // input_surface = surface;
}

static void wl_pointer_leave(void *data, struct wl_pointer *wl_pointer,
        uint32_t serial, struct wl_surface *surface) {
    // cur_x = cur_y = -1;
    // buttons = 0;
}

static void wl_pointer_motion(void *data, struct wl_pointer *wl_pointer,
        uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    int cur_x = wl_fixed_to_int(surface_x);
    int cur_y = wl_fixed_to_int(surface_y);

    printf(">>>>>: %d, %d\n", cur_x, cur_y);
}

static void wl_pointer_button(void *data, struct wl_pointer *wl_pointer,
        uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    printf(">>>>>: %d, %d\n", button, state);
    // if (input_surface == wl_surface) {
    //     if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
    //         if (button == BTN_RIGHT) {
    //             if (popup_wl_surface) {
    //                 popup_destroy();
    //             } else {
    //                 create_popup(serial);
    //             }
    //         } else {
    //             buttons++;
    //         }
    //     } else {
    //         if (button != BTN_RIGHT) {
    //             buttons--;
    //         }
    //     }
    // } else if (input_surface == popup_wl_surface) {
    //     if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
    //         if (button == BTN_LEFT && popup_red <= 0.9f) {
    //             popup_red += 0.1;
    //         } else if (button == BTN_RIGHT && popup_red >= 0.1f) {
    //             popup_red -= 0.1;
    //         }
    //     }
    // } else {
    //     assert(false && "Unknown surface");
    // }
}

static void wl_pointer_axis(void *data, struct wl_pointer *wl_pointer,
        uint32_t time, uint32_t axis, wl_fixed_t value) {
    // Who cares
}

static void wl_pointer_frame(void *data, struct wl_pointer *wl_pointer) {
    // Who cares
}

static void wl_pointer_axis_source(void *data, struct wl_pointer *wl_pointer,
        uint32_t axis_source) {
    // Who cares
}

static void wl_pointer_axis_stop(void *data, struct wl_pointer *wl_pointer,
        uint32_t time, uint32_t axis) {
    // Who cares
}

static void wl_pointer_axis_discrete(void *data, struct wl_pointer *wl_pointer,
        uint32_t axis, int32_t discrete) {
    // Who cares
}

static struct wl_pointer_listener pointer_listener = {
    .enter = wl_pointer_enter,
    .leave = wl_pointer_leave,
    .motion = wl_pointer_motion,
    .button = wl_pointer_button,
    .axis = wl_pointer_axis,
    .frame = wl_pointer_frame,
    .axis_source = wl_pointer_axis_source,
    .axis_stop = wl_pointer_axis_stop,
    .axis_discrete = wl_pointer_axis_discrete,
};


static void wl_keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard,
        uint32_t format, int32_t fd, uint32_t size) {
    // Who cares
}

static void wl_keyboard_enter(void *data, struct wl_keyboard *wl_keyboard,
        uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {
    fprintf(stderr, "Keyboard enter\n");
}

static void wl_keyboard_leave(void *data, struct wl_keyboard *wl_keyboard,
        uint32_t serial, struct wl_surface *surface) {
    fprintf(stderr, "Keyboard leave\n");
}

static void wl_keyboard_key(void *data, struct wl_keyboard *wl_keyboard,
        uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
    fprintf(stdout, "Key event: %d %d\n", key, state);
}

static void wl_keyboard_modifiers(void *data, struct wl_keyboard *wl_keyboard,
        uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched,
        uint32_t mods_locked, uint32_t group) {
    // Who cares
}

static void wl_keyboard_repeat_info(void *data, struct wl_keyboard *wl_keyboard,
        int32_t rate, int32_t delay) {
    // Who cares
}

static struct wl_keyboard_listener keyboard_listener = {
    .keymap = wl_keyboard_keymap,
    .enter = wl_keyboard_enter,
    .leave = wl_keyboard_leave,
    .key = wl_keyboard_key,
    .modifiers = wl_keyboard_modifiers,
    .repeat_info = wl_keyboard_repeat_info,
};


static void wl_touch_down(void *data, struct wl_touch *wl_touch, 
        uint32_t serial, uint32_t time, struct wl_surface *surface,
        int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{

}

static void wl_touch_up(void *data, struct wl_touch *wl_touch,
        uint32_t serial, uint32_t time, int32_t id)
{

}

static void wl_touch_motion(void *data, struct wl_touch *wl_touch,
        uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{

}

static void wl_touch_frame(void *data, struct wl_touch *wl_touch)
{

}

static void wl_touch_cancel(void *data, struct wl_touch *wl_touch)
{

}

// static void wl_touch_release(void *data, struct wl_touch *wl_touch)
// {

// }

static void wl_touch_shape(void *data, struct wl_touch *wl_touch,
        int32_t id, wl_fixed_t major, wl_fixed_t minor)
{

}

static void wl_touch_orientation(void *data, struct wl_touch *wl_touch,
        int32_t id, wl_fixed_t orientation)
{

}

static struct wl_touch_listener touch_listener = {
    .down = wl_touch_down,
    .up = wl_touch_up,
    .motion = wl_touch_motion,
    .frame = wl_touch_frame,
    .cancel = wl_touch_cancel,
    // .release = wl_touch_release,
    .shape = wl_touch_shape,
    .orientation = wl_touch_orientation,
};

static void seat_handle_capabilities(void *data, struct wl_seat *wl_seat,
        enum wl_seat_capability caps) {

    struct wl_context *ctx = data;
    
    if ((caps & WL_SEAT_CAPABILITY_POINTER)) {
        ctx->pointer = wl_seat_get_pointer(wl_seat);
        wl_pointer_add_listener(ctx->pointer, &pointer_listener, data);
    }

    if ((caps & WL_SEAT_CAPABILITY_KEYBOARD)) {
        ctx->keyboard = wl_seat_get_keyboard(wl_seat);
        wl_keyboard_add_listener(ctx->keyboard, &keyboard_listener, data);
    }

    if ((caps & WL_SEAT_CAPABILITY_TOUCH)) {
        ctx->touch = wl_seat_get_touch(wl_seat);
        wl_touch_add_listener(ctx->touch, &touch_listener, data);
    }
}

static void seat_handle_name(void *data, struct wl_seat *wl_seat,
        const char *name) {
}

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
    .name = seat_handle_name,
};


void input_pointer_init(struct wl_context *ctx)
{
    struct wl_cursor_theme *cursor_theme =
        wl_cursor_theme_load(NULL, 16, ctx->shm);
    assert(cursor_theme);
    struct wl_cursor *cursor =
        wl_cursor_theme_get_cursor(cursor_theme, "crosshair");
    if (cursor == NULL) {
        cursor = wl_cursor_theme_get_cursor(cursor_theme, "left_ptr");
    }
    assert(cursor);
    ctx->cursor_image = cursor->images[0];

    cursor = wl_cursor_theme_get_cursor(cursor_theme, "tcross");
    if (cursor == NULL) {
        cursor = wl_cursor_theme_get_cursor(cursor_theme, "left_ptr");
    }
    assert(cursor);
    ctx->popup_cursor_image = cursor->images[0];

    ctx->cursor_surface = wl_compositor_create_surface(ctx->wlCompositor);
    assert(ctx->cursor_surface);
}

void input_add_listener(struct wl_context *ctx)
{
    wl_seat_add_listener(ctx->seat, &seat_listener, ctx);
}

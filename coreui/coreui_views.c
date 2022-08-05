#include "coreui_server.h"
#include "coreui_view.h"
#include "include/coreui_view.h"
#include "shell/coreui_xdg_shell.h"
#include "shell/coreui_layer_shell.h"

#include <wlr/types/wlr_scene.h>

#include <assert.h>
#include <stdlib.h>


int coreui_view_init(struct coreui_server *server)
{
    wl_list_init(&server->views);

    coreui_xdg_shell_init(server);
    coreui_layer_shell_init(server);

    return 0;
}

void coreui_view_focus(struct coreui_view *view, struct wlr_surface *surface) {
    /* Note: this function only deals with keyboard focus. */
    if (view == NULL) {
        return;
    }
    struct coreui_server *server = view->server;
    struct wlr_seat *seat = server->seat;
    struct wlr_surface *prev_surface = seat->keyboard_state.focused_surface;
    if (prev_surface == surface) {
        /* Don't re-focus an already focused surface. */
        return;
    }
    if (prev_surface) {
        /*
         * Deactivate the previously focused surface. This lets the client know
         * it no longer has focus and the client will repaint accordingly, e.g.
         * stop displaying a caret.
         */
        struct wlr_xdg_surface *previous = wlr_xdg_surface_from_wlr_surface(
                    seat->keyboard_state.focused_surface);
        assert(previous->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL);
        wlr_xdg_toplevel_set_activated(previous->toplevel, false);
    }
    struct wlr_keyboard *keyboard = wlr_seat_get_keyboard(seat);
    /* Move the view to the front */
    wlr_scene_node_raise_to_top(&view->scene_tree->node);
    wl_list_remove(&view->link);
    wl_list_insert(&server->views, &view->link);

    /* Activate the new surface */
    if (view->type == COREUI_VIEW_XDG_SHELL)
        wlr_xdg_toplevel_set_activated(view->view.xdg_toplevel, true);
    /*
     * Tell the seat to have the keyboard enter this surface. wlroots will keep
     * track of this and automatically send key events to the appropriate
     * clients without additional work on your part.
     */
    if (keyboard != NULL && view->type == COREUI_VIEW_XDG_SHELL) {
        wlr_seat_keyboard_notify_enter(seat, view->view.xdg_toplevel->base->surface,
            keyboard->keycodes, keyboard->num_keycodes, &keyboard->modifiers);
    }
}
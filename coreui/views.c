#include "server.h"
#include "shell/layer-shell.h"

#include <stdlib.h>


static void xdg_surface_map(struct wl_listener *listener, void *data) {
    /* Called when the surface is mapped, or ready to display on-screen. */
    struct playos_view *view = wl_container_of(listener, view, map);
    view->mapped = true;
    playos_view_focus(view, view->xdg_surface->surface);
}

static void xdg_surface_unmap(struct wl_listener *listener, void *data) {
    /* Called when the surface is unmapped, and should no longer be shown. */
    struct playos_view *view = wl_container_of(listener, view, unmap);
    view->mapped = false;
}

static void xdg_surface_destroy(struct wl_listener *listener, void *data) {
    /* Called when the surface is destroyed and should never be shown again. */
    struct playos_view *view = wl_container_of(listener, view, destroy);
    wl_list_remove(&view->link);
    free(view);
}

static void begin_interactive(struct playos_view *view,
        enum playos_cursor_mode mode, uint32_t edges) {
    /* This function sets up an interactive move or resize operation, where the
     * compositor stops propegating pointer events to clients and instead
     * consumes them itself, to move or resize windows. */
    struct playos_server *server = view->server;
    struct wlr_surface *focused_surface =
        server->seat->pointer_state.focused_surface;
    if (view->xdg_surface->surface != focused_surface) {
        /* Deny move/resize requests from unfocused clients. */
        return;
    }
    server->grabbed_view = view;
    server->cursor_mode = mode;

    if (mode == PLAYOS_CURSOR_MOVE) {
        server->grab_x = server->cursor->x - view->x;
        server->grab_y = server->cursor->y - view->y;
    } else {
        struct wlr_box geo_box;
        wlr_xdg_surface_get_geometry(view->xdg_surface, &geo_box);

        double border_x = (view->x + geo_box.x) + ((edges & WLR_EDGE_RIGHT) ? geo_box.width : 0);
        double border_y = (view->y + geo_box.y) + ((edges & WLR_EDGE_BOTTOM) ? geo_box.height : 0);
        server->grab_x = server->cursor->x - border_x;
        server->grab_y = server->cursor->y - border_y;

        server->grab_geobox = geo_box;
        server->grab_geobox.x += view->x;
        server->grab_geobox.y += view->y;

        server->resize_edges = edges;
    }
}

static void xdg_toplevel_request_move(
        struct wl_listener *listener, void *data) {
    /* This event is raised when a client would like to begin an interactive
     * move, typically because the user clicked on their client-side
     * decorations. Note that a more sophisticated compositor should check the
     * provied serial against a list of button press serials sent to this
     * client, to prevent the client from requesting this whenever they want. */
    struct playos_view *view = wl_container_of(listener, view, request_move);
    begin_interactive(view, PLAYOS_CURSOR_MOVE, 0);
}

static void xdg_toplevel_request_resize(
        struct wl_listener *listener, void *data) {
    /* This event is raised when a client would like to begin an interactive
     * resize, typically because the user clicked on their client-side
     * decorations. Note that a more sophisticated compositor should check the
     * provied serial against a list of button press serials sent to this
     * client, to prevent the client from requesting this whenever they want. */
    struct wlr_xdg_toplevel_resize_event *event = data;
    struct playos_view *view = wl_container_of(listener, view, request_resize);
    begin_interactive(view, PLAYOS_CURSOR_RESIZE, event->edges);
}

static void server_new_xdg_surface(struct wl_listener *listener, void *data) {
    /* This event is raised when wlr_xdg_shell receives a new xdg surface from a
     * client, either a toplevel (application window) or popup. */
    struct playos_server *server =
        wl_container_of(listener, server, new_xdg_surface);
    struct wlr_xdg_surface *xdg_surface = (struct wlr_xdg_surface *)data;
    if (xdg_surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
        return;
    }

    /* Allocate a playos_view for this surface */
    struct playos_view *view =
        (struct playos_view *)calloc(1, sizeof(struct playos_view));
    view->server = server;
    view->xdg_surface = xdg_surface;

    /* Listen to the various events it can emit */
    view->map.notify = xdg_surface_map;
    wl_signal_add(&xdg_surface->events.map, &view->map);
    view->unmap.notify = xdg_surface_unmap;
    wl_signal_add(&xdg_surface->events.unmap, &view->unmap);
    view->destroy.notify = xdg_surface_destroy;
    wl_signal_add(&xdg_surface->events.destroy, &view->destroy);

    /* cotd */
    struct wlr_xdg_toplevel *toplevel = xdg_surface->toplevel;
    view->request_move.notify = xdg_toplevel_request_move;
    wl_signal_add(&toplevel->events.request_move, &view->request_move);
    view->request_resize.notify = xdg_toplevel_request_resize;
    wl_signal_add(&toplevel->events.request_resize, &view->request_resize);

    /* Add it to the list of views. */
    wl_list_insert(&server->views, &view->link);
}

int playos_views_init(struct playos_server *server)
{
    /* Set up our list of views and the xdg-shell. The xdg-shell is a Wayland
     * protocol which is used for application windows. For more detail on
     * shells, refer to my article:
     *
     * https://drewdevault.com/2018/07/29/Wayland-shells.html
     */
    wl_list_init(&server->views);
    wl_list_init(&server->ui_views);
    server->xdg_shell = wlr_xdg_shell_create(server->wl_display);
    server->new_xdg_surface.notify = server_new_xdg_surface;
    wl_signal_add(&server->xdg_shell->events.new_surface,
            &server->new_xdg_surface);

    playos_layer_shell_init(server);

    return 0;
}

void playos_view_focus(struct playos_view *view, struct wlr_surface *surface) {
    /* Note: this function only deals with keyboard focus. */
    if (view == NULL) {
        return;
    }
    struct playos_server *server = view->server;
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
        wlr_xdg_toplevel_set_activated(previous, false);
    }
    struct wlr_keyboard *keyboard = wlr_seat_get_keyboard(seat);
    /* Move the view to the front */
    wl_list_remove(&view->link);
    wl_list_insert(&server->views, &view->link);
    /* Activate the new surface */
    wlr_xdg_toplevel_set_activated(view->xdg_surface, true);
    /*
     * Tell the seat to have the keyboard enter this surface. wlroots will keep
     * track of this and automatically send key events to the appropriate
     * clients without additional work on your part.
     */
    wlr_seat_keyboard_notify_enter(seat, view->xdg_surface->surface,
        keyboard->keycodes, keyboard->num_keycodes, &keyboard->modifiers);
}

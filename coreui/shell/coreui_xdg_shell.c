#include "shell/coreui_xdg_shell.h"

#include "coreui_output_manager.h"
#include "coreui_view.h"
#include "coreui_output.h"

#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>

#include <assert.h>
#include <stdlib.h>


struct coreui_xdg_view {
    struct coreui_view view;

    struct wlr_xdg_toplevel *xdg_toplevel;

    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
    struct wl_listener request_fullscreen;
};

static struct coreui_xdg_view* coreui_xdg_view_from_view(struct coreui_view *view)
{
    return (struct coreui_xdg_view *)(view);
}

static struct wlr_scene_tree *xdg_create_subtree(struct coreui_view *view,
        struct wlr_scene_tree *parent)
{
    struct coreui_xdg_view *_view = coreui_xdg_view_from_view(view);

    view->scene_tree = wlr_scene_xdg_surface_create(
            parent, _view->xdg_toplevel->base);
    view->scene_tree->node.data = _view;
}

static void xdg_toplevel_map(struct wl_listener *listener, void *data) {
    struct coreui_xdg_view *view = wl_container_of(listener, view, map);
    struct coreui_output *output = coreui_output_manager_get_active_output(
            view->view.server->output_manager);

    coreui_output_add_view_with_subtree_creator(output, &view->view,
            Layer_App, xdg_create_subtree);

    view->xdg_toplevel->base->data = view->view.scene_tree;
    wlr_log(WLR_DEBUG, ">>>>>>>>>: %d\n", coreui_output_get_panel_height(output));

    coreui_view_focus(&view->view);
}

static void xdg_toplevel_unmap(struct wl_listener *listener, void *data) {
    struct coreui_xdg_view *view = wl_container_of(listener, view, unmap);


}

static void xdg_toplevel_destroy(struct wl_listener *listener, void *data) {
    struct coreui_xdg_view *view = wl_container_of(listener, view, destroy);

    wl_list_remove(&view->map.link);
    wl_list_remove(&view->unmap.link);
    wl_list_remove(&view->destroy.link);
    wl_list_remove(&view->request_move.link);
    wl_list_remove(&view->request_resize.link);
    wl_list_remove(&view->request_maximize.link);
    wl_list_remove(&view->request_fullscreen.link);

    free(view);
}

static void begin_interactive(struct coreui_xdg_view *view,
        enum coreui_cursor_mode mode, uint32_t edges) {
    /* This function sets up an interactive move or resize operation, where the
     * compositor stops propegating pointer events to clients and instead
     * consumes them itself, to move or resize windows. */
    struct coreui_server *server = view->view.server;
    struct wlr_surface *focused_surface =
            server->seat->pointer_state.focused_surface;
    if (view->xdg_toplevel->base->surface !=
            wlr_surface_get_root_surface(focused_surface)) {
        return;
    }
    server->grabbed_view = &view->view;
    server->cursor_mode = mode;

    if (mode == COREUI_CURSOR_MOVE) {
        server->grab_x = server->cursor->x - view->view.x;
        server->grab_y = server->cursor->y - view->view.y;
    } else {
        struct wlr_box geo_box;
        wlr_xdg_surface_get_geometry(view->xdg_toplevel->base, &geo_box);

        double border_x = (view->view.x + geo_box.x) +
            ((edges & WLR_EDGE_RIGHT) ? geo_box.width : 0);
        double border_y = (view->view.y + geo_box.y) +
            ((edges & WLR_EDGE_BOTTOM) ? geo_box.height : 0);
        server->grab_x = server->cursor->x - border_x;
        server->grab_y = server->cursor->y - border_y;

        server->grab_geobox = geo_box;
        server->grab_geobox.x += view->view.x;
        server->grab_geobox.y += view->view.y;

        server->resize_edges = edges;
    }
}

static void xdg_toplevel_request_move(
        struct wl_listener *listener, void *data) {
    /* This event is raised when a client would like to begin an interactive
     * move, typically because the user clicked on their client-side
     * decorations. Note that a more sophisticated compositor should check the
     * provided serial against a list of button press serials sent to this
     * client, to prevent the client from requesting this whenever they want. */
    struct coreui_xdg_view *view = wl_container_of(listener, view, request_move);
    begin_interactive(view, COREUI_CURSOR_MOVE, 0);
}

static void xdg_toplevel_request_resize(
        struct wl_listener *listener, void *data) {
    /* This event is raised when a client would like to begin an interactive
     * resize, typically because the user clicked on their client-side
     * decorations. Note that a more sophisticated compositor should check the
     * provided serial against a list of button press serials sent to this
     * client, to prevent the client from requesting this whenever they want. */
    struct wlr_xdg_toplevel_resize_event *event = data;
    struct coreui_xdg_view *view = wl_container_of(listener, view, request_resize);
    begin_interactive(view, COREUI_CURSOR_RESIZE, event->edges);
}

static void xdg_toplevel_request_maximize(
        struct wl_listener *listener, void *data) {
    /* This event is raised when a client would like to maximize itself,
     * typically because the user clicked on the maximize button on
     * client-side decorations. tinywl doesn't support maximization, but
     * to conform to xdg-shell protocol we still must send a configure.
     * wlr_xdg_surface_schedule_configure() is used to send an empty reply. */
    struct coreui_xdg_view *view =
            wl_container_of(listener, view, request_maximize);
    wlr_xdg_surface_schedule_configure(view->xdg_toplevel->base);
}

static void xdg_toplevel_request_fullscreen(
        struct wl_listener *listener, void *data) {
    /* Just as with request_maximize, we must send a configure here. */
    struct coreui_xdg_view *view =
            wl_container_of(listener, view, request_fullscreen);
    wlr_xdg_surface_schedule_configure(view->xdg_toplevel->base);
}

static void coreui_xdg_view_focus(struct coreui_view *view)
{
    if (view == NULL) {
        return;
    }

    struct coreui_server *server = view->server;
    struct coreui_xdg_view *xview = coreui_xdg_view_from_view(view);
    struct wlr_seat *seat = server->seat;
    struct wlr_surface *surface = xview->xdg_toplevel->base->surface;
    struct wlr_surface *prev_surface = seat->keyboard_state.focused_surface;
    if (prev_surface == surface) {
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

    /* Activate the new surface */
    wlr_xdg_toplevel_set_activated(xview->xdg_toplevel, true);

    /*
     * Tell the seat to have the keyboard enter this surface. wlroots will keep
     * track of this and automatically send key events to the appropriate
     * clients without additional work on your part.
     */
    if (keyboard != NULL) {
        wlr_seat_keyboard_notify_enter(seat, xview->xdg_toplevel->base->surface,
            keyboard->keycodes, keyboard->num_keycodes, &keyboard->modifiers);
    }
}

static void coreui_xdg_view_resize(struct coreui_view *_view,
        int left, int right, int top, int bottom)
{
    struct coreui_xdg_view *view = coreui_xdg_view_from_view(_view);

    struct wlr_box geo_box;
    wlr_xdg_surface_get_geometry(view->xdg_toplevel->base, &geo_box);
    view->view.x = left - geo_box.x;
    view->view.y = top - geo_box.y;
    wlr_scene_node_set_position(&view->view.scene_tree->node, view->view.x, view->view.y);

    int new_width = right - left;
    int new_height = bottom - top;
    wlr_xdg_toplevel_set_size(view->xdg_toplevel, new_width, new_height);
}

static struct coreui_view_operations coreui_xdg_view_vops = {
    .focus = coreui_xdg_view_focus,
    .resize = coreui_xdg_view_resize,
};

static void server_new_xdg_surface(struct wl_listener *listener, void *data)
{
    struct coreui_xdg_shell *shell = 
            wl_container_of(listener, shell, new_xdg_surface);
    struct coreui_server *server = shell->server;
    struct wlr_xdg_surface *xdg_surface = data;

    /* We must add xdg popups to the scene graph so they get rendered. The
     * wlroots scene graph provides a helper for this, but to use it we must
     * provide the proper parent scene node of the xdg popup. To enable this,
     * we always set the user data field of xdg_surfaces to the corresponding
     * scene node. */
    if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
        struct wlr_xdg_surface *parent = wlr_xdg_surface_from_wlr_surface(
            xdg_surface->popup->parent);
        struct wlr_scene_tree *parent_tree = parent->data;
        xdg_surface->data = wlr_scene_xdg_surface_create(
            parent_tree, xdg_surface);
        return;
    }
    assert(xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL);


    struct coreui_xdg_view *view = 
            (struct coreui_xdg_view *) calloc(1, sizeof(*view));
    if (view == NULL) {
        return;
    }

    view->xdg_toplevel = xdg_surface->toplevel;

    coreui_view_init(&view->view, server, xdg_surface->surface);

    // Set functions
    view->view.vops = &coreui_xdg_view_vops;

    /* Listen to the various events it can emit */
    view->map.notify = xdg_toplevel_map;
    wl_signal_add(&xdg_surface->events.map, &view->map);
    view->unmap.notify = xdg_toplevel_unmap;
    wl_signal_add(&xdg_surface->events.unmap, &view->unmap);
    view->destroy.notify = xdg_toplevel_destroy;
    wl_signal_add(&xdg_surface->events.destroy, &view->destroy);

    /* cotd */
    struct wlr_xdg_toplevel *toplevel = xdg_surface->toplevel;
    view->request_move.notify = xdg_toplevel_request_move;
    wl_signal_add(&toplevel->events.request_move, &view->request_move);
    view->request_resize.notify = xdg_toplevel_request_resize;
    wl_signal_add(&toplevel->events.request_resize, &view->request_resize);
    view->request_maximize.notify = xdg_toplevel_request_maximize;
    wl_signal_add(&toplevel->events.request_maximize, &view->request_maximize);
    view->request_fullscreen.notify = xdg_toplevel_request_fullscreen;
    wl_signal_add(&toplevel->events.request_fullscreen, &view->request_fullscreen);
}

int coreui_xdg_shell_init(struct coreui_server *server)
{
    struct coreui_xdg_shell *shell = 
            (struct coreui_xdg_shell *) calloc(1, sizeof(*shell));
    if (shell == NULL) {
        return -1;
    }

    server->xdg_shell = shell;
    shell->server = server;

    shell->xdg_shell = wlr_xdg_shell_create(server->display, 3);
    shell->new_xdg_surface.notify = server_new_xdg_surface;
    wl_signal_add(&shell->xdg_shell->events.new_surface, &shell->new_xdg_surface);

    return 0;
}

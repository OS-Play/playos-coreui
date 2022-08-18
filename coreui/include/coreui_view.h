#ifndef __COREUI_VIEW_H__
#define __COREUI_VIEW_H__

#include <wayland-util.h>
#include "coreui_utils.h"

COREUI_EXTERN_BEGIN

struct coreui_view_operations {
    /**
     * @brief Invoke on view get focus
     * 
     */
    void (*focus)(struct coreui_view *view);
    /**
     * @brief Invoke on view get blur
     * 
     */
    void (*blur)(struct coreui_view *view);
    /**
     * @brief Invoke on view resize
     * 
     */
    void (*resize)(struct coreui_view *view, int left, int right, int top, int bottom);
    void (*move)(struct coreui_view *view, int x, int y);
    void (*on_drag)(struct coreui_view *view, int x, int y);
    void (*on_drop)(struct coreui_view *view, int x, int y);
};

struct coreui_view {
    struct wl_list link;
    struct coreui_server *server;
    struct coreui_output *output;

    struct wlr_scene_tree *scene_tree;
    struct wlr_surface *surface;

    int x, y;

    struct coreui_view_operations *vops;
};

int coreui_views_init(struct coreui_server *server);

void coreui_view_init(struct coreui_view *view, struct coreui_server *server,
        struct wlr_surface *surface);
void coreui_view_focus(struct coreui_view *view);
void coreui_view_blur(struct coreui_view *view);
void coreui_view_resize(struct coreui_view *view, int left, int right, int top, int bottom);

COREUI_EXTERN_END

#endif

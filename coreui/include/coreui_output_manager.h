#ifndef __COREUI_OUTPUT_MANAGER_H__
#define __COREUI_OUTPUT_MANAGER_H__

#include <wayland-server.h>

struct coreui_server;
struct coreui_output_manager;
struct coreui_output;
struct coreui_view;

struct wlr_output;

struct coreui_output_manager *coreui_output_manager_create(struct coreui_server *server);
void coreui_output_manager_destroy(struct coreui_output_manager *m);

void coreui_output_manager_remove_output(struct coreui_output_manager *m,
        struct coreui_output *output);

struct wlr_scene *coreui_output_manager_get_scene(struct coreui_output_manager *m);

struct coreui_output *coreui_output_manager_get_output(
        struct coreui_output_manager *m, struct wlr_output *output);
struct wlr_output_layout *coreui_output_manager_get_output_layout(
        struct coreui_output_manager *m);

struct coreui_output *coreui_output_manager_get_active_output(struct coreui_output_manager *m);

#endif

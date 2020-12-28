#ifndef __PLAYOS_OUTPUT_H__
#define __PLAYOS_OUTPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

struct coreui_output {
    struct wl_output *wl_output;

    struct wl_context *ctx;
    char name[64];
    char description[512];

    int width;
    int height;
    char currMode[128];
    char modes[64][128];
};

struct coreui_output *coreui_output_create(struct wl_context *ctx, struct wl_output *_output);
void coreui_output_destroy(struct coreui_output *output);

#ifdef __cplusplus
}
#endif

#endif

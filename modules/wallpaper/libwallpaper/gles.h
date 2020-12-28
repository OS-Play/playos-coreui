#ifndef __PLAYOS_GLES_H__
#define __PLAYOS_GLES_H__

#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libwallpaper.h"

void gles_init();
void gles_destroy();
void gles_set_texture(const void *data, int width, int height);
void gles_draw(struct pos_wallpaper *ctx);

void gles_get_projection(int width, int height, int iw, int ih, enum pos_scale_mode mode, float projection[16]);

#ifdef __cplusplus
}
#endif

#endif

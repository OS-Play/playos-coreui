#ifndef __PLAYOS_LIBWALLPAPER_H__
#define __PLAYOS_LIBWALLPAPER_H__

#ifdef __cplusplus
extern "C" {
#endif

struct pos_wallpaper;
struct wl_display;

enum pos_scale_mode {
    Fit,
    Fill,
    Center,
    Tile
};

/**
 * @brief Create a wallpaper context
 * 
 * @param display Can be null
 * @return struct pos_wallpaper *
 */
struct pos_wallpaper *pos_wallpaper_init(struct wl_display *display);

/**
 * @brief Destroy a wallpaper context
 * 
 * @param ctx struct pos_wallpaper *
 */
void pos_wallpaper_deinit(struct pos_wallpaper *ctx);

/**
 * @brief Run wallpapaer context once
 * 
 * @param ctx struct pos_wallpaper *
 * @return int sucess in 0, otherwise error
 */
int pos_wallpaper_run(struct pos_wallpaper *ctx);

/**
 * @brief Set wallpaper mode
 * 
 * @param ctx struct pos_wallpaper *
 * @param mode Wallpaper scale mode
 */
void pos_wallpaper_set_mode(struct pos_wallpaper *ctx, enum pos_scale_mode mode);

/**
 * @brief Set wallpaper image
 * 
 * @param ctx struct pos_wallpaper *
 * @param image Image data, in rgb pixels
 * @param width image width
 * @param height image height
 */
void pos_wallpaper_set_image(struct pos_wallpaper *ctx, void *image, int width, int height);


#ifdef __cplusplus
}
#endif

#endif

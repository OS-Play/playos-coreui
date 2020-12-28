#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdio.h>

#include "libwallpaper.h"

#include "sti_image.h"

int main(int argc, char *argv[])
{
    int ret;
    const char *wp_file = "/home/rany/project/build/usr/share/wayfire/wallpaper.jpg";
    struct pos_wallpaper *ctx = pos_wallpaper_init(NULL);

    if (ctx == NULL) {
        return -1;
    }

    if (argc == 2) {
        wp_file = argv[1];
    }

    if (argc == 3) {
        pos_wallpaper_set_mode(ctx, (enum pos_scale_mode)(argv[2][0] - '0'));
    } else {
        pos_wallpaper_set_mode(ctx, Fill);
    }

    int x,y,n;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(wp_file, &x, &y, &n, 0);

    pos_wallpaper_set_image(ctx, data, x, y);

    while (1) {
        ret = pos_wallpaper_run(ctx);
        if (ret != 0) {
            std::cout << "pos_wallpaper_run: " << strerror(ret) << ", " << strerror(errno) << "\n";
            break;
        }
    }

    stbi_image_free(data);

    pos_wallpaper_deinit(ctx);

    return 0;
}

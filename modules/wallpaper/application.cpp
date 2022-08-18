#include "application.h"

#include <stdio.h>
#include <iostream>
#include <memory>
#include <string.h>

#include <glad/glad.h>
#include <EGL/egl.h>
#include <sti_image.h>

#include "coreui/wayland/layer_shell_surface.h"

#define DEFAULT_WP "/home/rany/project/build/usr/share/wayfire/wallpaper.jpg"

namespace playos {

static void printHelp(const char *program)
{
    std::cout << "usage: " << program << " [options]\n" << R"(
options:
    --mode <mode> value: fit, fill, center, tile
    --image <image path>
)";
    exit(0);
}

#define _printHelp(p)  \
    printHelp(p); \
        break
#define checkOrPrintHelp(p) \
    if (argc <= ++i) { \
        _printHelp(p); \
    }

WallpaperApplication::WallpaperApplication(int argc, char **argv):
        Application(argc, argv), m_needRedraw(true),
        m_scaleMode(Wallpaper::Fit)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--mode") == 0) {
            checkOrPrintHelp(argv[0]);

            const char *mode = argv[i];
            if (strcmp(mode, "fit") == 0) {
                m_scaleMode = Wallpaper::Fit;
            } else if (strcmp(mode, "fill") == 0) {
                m_scaleMode = Wallpaper::Fill;
            } else if (strcmp(mode, "center") == 0) {
                m_scaleMode = Wallpaper::Center;
            } else if (strcmp(mode, "tile") == 0) {
                m_scaleMode = Wallpaper::Tile;
            } else {
                _printHelp(argv[0]);
            }
        } else if (strcmp(argv[i], "--image") == 0) {
            checkOrPrintHelp(argv[0]);

            m_imagePath = std::string(argv[i]);
        }
    }
}
#undef _printHelp
#undef checkOrPrintHelp

int WallpaperApplication::onInit()
{
    auto ctx = context();
    struct coreui_output *output = ctx->getCurrentOutput();
    playos::SurfaceFactory &factory = playos::SurfaceFactory::instance();
    auto surface = factory.createSurface(ctx, playos::CoreuiSurface::LayerShell);
    auto lsSurface = std::dynamic_pointer_cast<playos::LayerShellSurface>(surface);

    lsSurface->setSize(output->width, output->height);
    lsSurface->setLayer(playos::LayerShellSurface::LayerBackground);

    m_window.reset(createWindow(surface, "main", output->width, output->height));
    m_window->setDrawEventListener(this);

    m_window->makeCurrent();
    if (!gladLoadGLES2Loader((GLADloadproc)eglGetProcAddress)) {
        printf("Failed to load gles library\n");
        return -1;
    }

    m_wallpaper = std::make_unique<Wallpaper>(output->width, output->height);

    int x,y,n;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data;
    if (m_imagePath.empty()) {
        data = stbi_load(DEFAULT_WP, &x, &y, &n, 0);
    } else {
        data = stbi_load(m_imagePath.c_str(), &x, &y, &n, 0);
    }
    m_wallpaper->setImage(data, x, y);
    stbi_image_free(data);
    m_wallpaper->setScaleMode(m_scaleMode);
    m_wallpaper->setBgColor(0.0f, 1.0f, 0.0f, 1.0f);

    m_window->swapBuffer();
    return 0;
}

void WallpaperApplication::onDraw(Window *window, uint32_t time)
{
    if (!m_needRedraw) {
        return;
    }
    glViewport(0, 0, window->width(), window->height());

    m_wallpaper->draw();

    window->swapBuffer();
    m_needRedraw = false;
}

}
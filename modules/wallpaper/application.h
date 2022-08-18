#ifndef __PLAYOS_WP_APPLICATION_H__
#define __PLAYOS_WP_APPLICATION_H__

#include <memory>

#include "coreui/application.h"
#include "coreui/window.h"

#include "wallpaper.h"

namespace playos {

class WallpaperApplication: public Application, public DrawEventListener {
public:
    WallpaperApplication(int argc, char **argv);

    void onDraw(Window *window, uint32_t time);
    int onInit();

private:
    bool m_needRedraw;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Wallpaper> m_wallpaper;

    Wallpaper::ScaleMode m_scaleMode;
    std::string m_imagePath;
};

}

#endif

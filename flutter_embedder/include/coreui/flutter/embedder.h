#ifndef __PLAYOS_COREUI_EMBEDDER_H__
#define __PLAYOS_COREUI_EMBEDDER_H__

#include <string>

#include "coreui/window.h"

namespace playos {

bool RunFlutter(Window *window,
                const std::string& project_path,
                const std::string& icudtl_path,
                int argc, const char **argv);

void setFlutterWindowSize(Window *window, int width, int height);

}

#endif

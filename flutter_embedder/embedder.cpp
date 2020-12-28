#include "coreui/flutter/embedder.h"

#include "embedder.h"

#include <cstdio>
#include <iostream>
#include <string>

#include <EGL/egl.h>

namespace playos {

bool RunFlutter(Window *window,
                const std::string& assets_path,
                const std::string& icudtl_path,
                int argc, const char **argv) {
    FlutterRendererConfig config = {};
    window->createResourceContext();
    // window->makeCurrent();

    config.type = kOpenGL;
    config.open_gl.struct_size = sizeof(config.open_gl);
    config.open_gl.make_current = [](void* userdata) -> bool {
        Window *window = (static_cast<Window *>(userdata));
        window->makeCurrent();
        return true;
    };
    config.open_gl.clear_current = [](void* userdata) -> bool {
        Window *window = (static_cast<Window *>(userdata));
        window->clearCurrent();
        return true;
    };
    config.open_gl.present = [](void* userdata) -> bool {
        Window *window = (static_cast<Window *>(userdata));
        window->swapBuffer();
        return true;
    };
    config.open_gl.fbo_callback = [](void*) -> uint32_t {
        return 0;  // FBO0
    };
    config.open_gl.make_resource_current = [](void *userdata) -> bool {
        Window *window = (static_cast<Window *>(userdata));
        window->makeResourceCurrent();

        return true;
    };
    config.open_gl.gl_proc_resolver = [](void *userdata, const char* name) -> void* {
        Window *window = (static_cast<Window *>(userdata));

        return window->getProcAddress(name);
    };

    FlutterProjectArgs args = {
        .struct_size = sizeof(FlutterProjectArgs),
        .assets_path = assets_path.c_str(),
        .icu_data_path =
            icudtl_path.c_str(),  // Find this in your bin/cache directory.
        .command_line_argc = argc,
        .command_line_argv = argv,
    };
    FlutterEngine engine = nullptr;
    FlutterEngineResult result =
        FlutterEngineRun(FLUTTER_ENGINE_VERSION, &config,  // renderer
                        &args, window, &engine);
    if (result != kSuccess || engine == nullptr) {
        std::cout << "Could not run the Flutter Engine." << std::endl;
        return false;
    }

    window->setPriv(engine);
    setFlutterWindowSize(window, window->width(), window->height());

    return true;
}

void setFlutterWindowSize(Window *window, int width, int height)
{
    FlutterWindowMetricsEvent event = {};
    event.struct_size = sizeof(event);
    event.width = width;
    event.height = height;
    event.pixel_ratio = 1.0;
    FlutterEngineSendWindowMetricsEvent(
        reinterpret_cast<FlutterEngine>(window->priv()),
        &event);
}

}

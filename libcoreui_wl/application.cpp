#include "coreui/application.h"

#include "coreui/event_loop.h"
#include "coreui/window.h"
#include "../wayland/wl_context.hxx"
#include "coreui/wayland/layer_shell_surface.h"

#include <cstdlib>
#include <memory>

#include <limits.h>
#include <mutex>
#include <stdlib.h>


namespace playos {

Application::Application(int argc, char **argv):
        argc(argc), argv(argv)
{
}

Application::~Application()
{
}

int Application::init()
{
    int ret = 0;
    std::call_once(m_initFlag, [&]() {
        m_ctx.reset(WLContext::create());
        if (m_ctx == nullptr) {
            ret = -1;
        }
        m_loop.reset(EventLoop::create(m_ctx));
        if (m_loop == nullptr) {
            m_ctx = nullptr;
            ret = -1;
        }
    });

    return ret;
}

Window *Application::createWindow(const char *title, int width, int height)
{
    return Window::create(m_ctx, title, width, height);
}

Window *Application::createWindow(std::shared_ptr<CoreuiSurface> &surface, const char *title, int width, int height)
{
    return Window::create(m_ctx, surface, title, width, height);
}

int Application::run()
{
    int ret = 0;

    if ((ret = init()) != 0) {
        return ret;
    }

    return m_loop->exec();
}

std::string Application::getRealpath(const std::string &path)
{
    return getRealpath(path.c_str());
}

std::string Application::getRealpath(const char *path)
{
    char _path[PATH_MAX];

    realpath(path, _path);

    return std::string(_path);
}

std::string Application::execPath(const char *path)
{
    std::string rpath = getRealpath(path);

    size_t pos = rpath.rfind('/');
    return rpath.substr(0, pos);
}

}

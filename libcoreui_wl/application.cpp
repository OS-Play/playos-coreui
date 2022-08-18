#include "coreui/application.h"

#include "coreui/event_loop.h"
#include "coreui/window.h"
#include "coreui/log.h"
#include "../wayland/wl_context.hxx"
#include "coreui/wayland/layer_shell_surface.h"

#include <cstdlib>
#include <memory>

#include <limits.h>
#include <mutex>
#include <stdlib.h>
#include <sys/epoll.h>
#include <string.h>


namespace playos {

Application *Application::sApp = nullptr;

Application::Application(int argc, char **argv):
        argc(argc), argv(argv)
{
    sApp = this;
}

Application::~Application()
{
    if (m_ctx && m_loop)
        m_loop->removeWatchFd(m_ctx->getDisplayFd());
}

int Application::init()
{
    int ret = 0;
    std::call_once(m_initFlag, [&]() {
        m_ctx.reset(WLContext::create());
        if (m_ctx == nullptr) {
            ret = -1;
        }
        m_loop.reset(EventLoop::create());
        if (m_loop == nullptr) {
            m_ctx = nullptr;
            ret = -1;
        }

        m_displayEvent.run = std::bind(&Application::handleDisplayEvent, this,
                std::placeholders::_1, std::placeholders::_2);
        m_displayFDPTask.run = std::bind(&Application::displayflushAndDispatchPending, this,
                std::placeholders::_1, std::placeholders::_2);

        if (m_loop->addWatchFd(m_ctx->getDisplayFd(), &m_displayEvent) != 0) {
            ret = -1;
        }

        m_loop->post(&m_displayFDPTask, Task::Loop);

        if (onInit() != 0) {
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


void Application::handleDisplayEvent(Task *task, int events)
{
    int ret = 0;

    if (events & EPOLLIN) {
        ret = m_ctx->dispatch();
        if (ret == -1) {
            LOG_ERROR("Application", "Context dispatch error: %s\n", strerror(errno));
            m_loop->exit();
            return;
        }
    }

    if (events & EPOLLOUT) {
        m_ctx->flush();
    }
}

void Application::displayflushAndDispatchPending(Task *task, int events)
{
    m_ctx->dispatchPending();
    m_ctx->flush();
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

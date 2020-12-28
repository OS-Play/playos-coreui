#ifndef __COREUI_APPLICATION_H__
#define __COREUI_APPLICATION_H__

#include "event_loop.h"
#include "coreui/wayland/surface.h"
#include "coreui/window.h"
#include <memory>
#include <mutex>
#include <string>

namespace playos {

class Application {
protected:
    virtual int update() = 0;
    virtual int draw() = 0;

public:
    virtual int init();

public:
    Application(int argc, char **argv);
    virtual ~Application();

    std::shared_ptr<WLContext> context() {
        return m_ctx;
    }

    EventLoop *eventLoop() {
        return m_loop.get();
    }

    int run();

    Window *createWindow(const char *title, int width, int height);
    Window *createWindow(std::shared_ptr<CoreuiSurface> &surface, const char *title, int width, int height);

public:
    static std::string getRealpath(const std::string &path);
    static std::string getRealpath(const char *path);
    static std::string execPath(const char *path);

protected:
    int argc;
    char **argv;

private:
    std::shared_ptr<WLContext> m_ctx;
    std::unique_ptr<EventLoop> m_loop;

    std::once_flag m_initFlag;
};

}

#endif

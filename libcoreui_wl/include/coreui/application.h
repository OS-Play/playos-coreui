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
public:
    int init();

    static Application *instance() {
        return sApp;
    }

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

    void handleDisplayEvent(Task *task, int events);
    void displayflushAndDispatchPending(Task *task, int events);
public:
    static std::string getRealpath(const std::string &path);
    static std::string getRealpath(const char *path);
    static std::string execPath(const char *path);

protected:
    virtual int onInit() { return 0; }

protected:
    int argc;
    char **argv;

private:
    std::shared_ptr<WLContext> m_ctx;
    std::unique_ptr<EventLoop> m_loop;
    Task m_displayEvent;
    Task m_displayFDPTask;

    std::once_flag m_initFlag;

private:
    static Application *sApp;
};

}

#endif

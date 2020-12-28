#ifndef __PLAYOS_EVENT_LOOP_H__
#define __PLAYOS_EVENT_LOOP_H__

#include "../../wayland/wl_context.hxx"

#include <functional>
#include <memory>


namespace playos {

class EventLoop;

class Task {
public:
    Task() { };
    Task(const std::function<void(Task *task, int events)> &run);

public:
    struct wl_list link;
    std::function<void(Task *task, int events)> run;

    friend class EventLoop;
};

class EventLoop {
private:
    EventLoop(std::shared_ptr<WLContext> &ctx);
public:
    static EventLoop *create(std::shared_ptr<WLContext> &ctx);
    ~EventLoop();

    EventLoop(EventLoop const&) = delete;
    EventLoop& operator=(EventLoop const&) = delete;

    std::shared_ptr<WLContext> context() { return ctx; }

    int init();

    int exec();
    void exit();

    void post(Task *task);
    int addWatchFd(int fd, Task *task);
    void removeWatchFd(int fd);
    void handleDisplayEvent(Task *task, int events);

private:
    std::shared_ptr<WLContext> ctx;

    int m_sock[2];
    int m_epoll;
    bool m_running;
    struct wl_list postTasks;

    Task m_displayEvent;
};

}

#endif

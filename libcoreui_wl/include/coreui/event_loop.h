#ifndef __PLAYOS_EVENT_LOOP_H__
#define __PLAYOS_EVENT_LOOP_H__

#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <sys/epoll.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif
#include <wayland-util.h>


namespace playos {

class EventLoop;

class Task {
public:
    enum Mode {
        Once,
        Loop
    };

    Task() { };
    Task(const std::function<void(Task *task, int events)> &run);

public:
    struct wl_list link;
    std::function<void(Task *task, int events)> run;
    Mode mode;
    void *data;

    friend class EventLoop;
};

class EventLoop {
private:
    EventLoop();

    int init();

public:
    static EventLoop *create();
    ~EventLoop();

    EventLoop(EventLoop const&) = delete;
    EventLoop& operator=(EventLoop const&) = delete;

    bool runOnCurrentThread();

    int exec();
    void exit();

    void post(Task *task, Task::Mode mode = Task::Once);
    int addWatchFd(int fd, Task *task, int events = EPOLLIN | EPOLLERR | EPOLLHUP);
    void removeWatchFd(int fd);

private:
    int m_sock[2];
    int m_epoll;
    bool m_running;
    std::mutex m_mutex;
    struct wl_list m_postTasks;
    std::thread::id m_mainThreadId;
};

}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif

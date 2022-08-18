#include "coreui/event_loop.h"
#include "coreui/log.h"

#include <string.h>
#include <errno.h>
#include <memory>

#include <GLES2/gl2.h>
#include <stdio.h>

#include <unistd.h>


namespace playos {

Task::Task(const std::function<void(Task *task, int events)> &cb): run(cb)
{
}

EventLoop::EventLoop():m_running(false)
{
}

EventLoop *EventLoop::create()
{
    EventLoop *loop = new EventLoop();
    if (loop->init()) {
        delete loop;
        return nullptr;
    }

    return loop;
}

EventLoop::~EventLoop()
{
    exit();
    close(m_epoll);
}

int EventLoop::init()
{
    wl_list_init(&m_postTasks);

    m_epoll = epoll_create1(EPOLL_CLOEXEC);
    if (m_epoll <= 0) {
        return -1;
    }
    m_mainThreadId = std::this_thread::get_id();

    return 0;
}

bool EventLoop::runOnCurrentThread()
{
    return m_mainThreadId == std::this_thread::get_id();
}

int EventLoop::exec()
{
    int ret = 0, count;
    Task *task, *tmp;
    struct epoll_event evs[16];

    m_running = true;
    while (m_running) {

        wl_list_for_each_safe(task, tmp, &m_postTasks, link) {
            if (task->mode == Task::Once)
                wl_list_remove(&task->link);
            task->run(task, 0);
        }

        count = epoll_wait(m_epoll, evs, sizeof(evs)/sizeof(struct epoll_event), -1);
        for (int i = 0; i < count; i++) {
            task = (Task *)evs[i].data.ptr;
            task->run(task, evs[i].events);
        }
    }

    return ret;
}

void EventLoop::post(Task *task, Task::Mode mode)
{
    if (!runOnCurrentThread()) {
        m_mutex.lock();
    }

    task->mode = mode;
    wl_list_insert(&m_postTasks, &task->link);

    if (!runOnCurrentThread()) {
        m_mutex.unlock();
    }
}

int EventLoop::addWatchFd(int fd, Task *task, int events)
{
    struct epoll_event event;
    event.data.ptr = task;
    event.events = events;

    return epoll_ctl(m_epoll, EPOLL_CTL_ADD, fd, &event);
}

void EventLoop::removeWatchFd(int fd)
{
    epoll_ctl(m_epoll, EPOLL_CTL_DEL, fd, NULL);
}

void EventLoop::exit()
{
    m_running = false;
}

}

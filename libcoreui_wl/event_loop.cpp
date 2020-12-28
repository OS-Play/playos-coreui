#include "coreui/event_loop.h"
#include "coreui/log.h"

#include "wayland/wl_context.hxx"

#include <string.h>
#include <errno.h>
#include <memory>

#include <GLES2/gl2.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/epoll.h>

namespace playos {

Task::Task(const std::function<void(Task *task, int events)> &cb): run(cb)
{
}

EventLoop::EventLoop(std::shared_ptr<WLContext> &ctx):m_running(false), ctx(ctx)
{
}

EventLoop *EventLoop::create(std::shared_ptr<WLContext> &ctx)
{
    EventLoop *loop = new EventLoop(ctx);
    if (loop->init()) {
        delete loop;
        return nullptr;
    }

    return loop;
}

EventLoop::~EventLoop()
{
    exit();
    removeWatchFd(ctx->getDisplayFd());
    close(m_epoll);
}

int EventLoop::init()
{
    wl_list_init(&postTasks);

    m_epoll = epoll_create1(EPOLL_CLOEXEC);
    if (m_epoll <= 0) {
        return -1;
    }

    m_displayEvent.run = std::bind(&EventLoop::handleDisplayEvent, this,
        std::placeholders::_1, std::placeholders::_2);

    if (addWatchFd(ctx->getDisplayFd(), &m_displayEvent) != 0) {
        return -1;
    }

    return 0;
}

int EventLoop::exec()
{
    int ret = 0, count;
    Task *task;
    struct epoll_event evs[16];

    if (ctx == nullptr) {
        return -1;
    }

    m_running = true;
    while (m_running) {
        while (!wl_list_empty(&postTasks)) {
            task = wl_container_of(postTasks.prev, task, link);
            wl_list_remove(&task->link);
            task->run(task, 0);
        }

        ctx->dispatchPending();

        count = epoll_wait(m_epoll, evs, sizeof(evs)/sizeof(struct epoll_event), -1);
        for (int i = 0; i < count; i++) {
            task = (Task *)evs[i].data.ptr;
            task->run(task, evs[i].events);
        }
    }

    return ret;
}

void EventLoop::post(Task *task)
{
    wl_list_insert(&postTasks, &task->link);
}

int EventLoop::addWatchFd(int fd, Task *task)
{
    struct epoll_event event;
    event.data.ptr = task;
    event.events = EPOLLIN | EPOLLOUT | EPOLLHUP;

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

void EventLoop::handleDisplayEvent(Task *task, int events)
{
    int ret = 0;

    if (events & EPOLLIN) {
        ret = ctx->dispatch();
        if (ret == -1) {
            LOG_ERROR("EventLoop", "Context dispatch error: %s\n", strerror(errno));
            m_running = false;
            return;
        }
    }

    if (events & EPOLLOUT) {
        ctx->flush();
    }
}

}

#ifndef __PLAYOS_WINDOW_EVENT_H__
#define __PLAYOS_WINDOW_EVENT_H__

#include <memory>

namespace playos {

class Event;

typedef std::shared_ptr<Event> EventPtr;

enum EventType {
    WINDOW_CLOSE,
    WINDOW_RESIZE,
    WINDOW_MIMIMUM,
    WINDOW_MAXIMUM,
    WINDOW_HIDDEN,
    WINDOW_SHOW,
    WINDOW_FRAME,
};

class Event: public std::enable_shared_from_this<Event> {
public:
    Event() { }
    Event(EventType type): type(type) { }
    virtual ~Event() { }

    Event(const Event &) = delete;
    Event &operator=(const Event &) = delete;

    template<typename T, typename... Args>
    static EventPtr create(Args... args) {
        return std::make_shared<T>(args...);
    }

    template<typename T>
    std::shared_ptr<T> cast();

    EventType type;
};

class ResizeEvent: public Event {
public:
    ResizeEvent(EventType type, int width, int height):
        Event(type), width(width), height(height) { }

    int width, height;
};

class FrameEvent: public Event {
public:
    FrameEvent(EventType type, uint32_t time):
        Event(type), time(time) { }

    uint32_t time;
};

template<typename T>
std::shared_ptr<T> Event::cast()
{
    return std::dynamic_pointer_cast<T>(shared_from_this());
}

}

#endif

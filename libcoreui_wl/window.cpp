#include "coreui/window.h"
#include "coreui/wayland/surface.h"
#include "coreui/window.h"
#include "wayland/egl_wayland.h"
#include "wayland/wl_context.hxx"

#include <string.h>


namespace playos {

Window::Window(std::shared_ptr<WLContext> &ctx, const char *title, int width, int height):
    m_ctx(ctx), m_width(width), m_height(height), m_mouseListener(nullptr),
    m_keyboardListener(nullptr), m_windowEventListener(nullptr),
    m_drawEventListener(nullptr)
{
    strncpy(this->m_title, title, sizeof(this->m_title));
}

Window *Window::create(std::shared_ptr<WLContext> &ctx, const char *title, int width, int height)
{
    SurfaceFactory &factory = SurfaceFactory::instance();

    auto surface = factory.createSurface(ctx, CoreuiSurface::Default);

    return create(ctx, surface, title, width, height);
}

Window *Window::create(std::shared_ptr<WLContext> &ctx, std::shared_ptr<CoreuiSurface> &surface, const char *title, int width, int height)
{
    struct coreui_output *output = nullptr;
    if (surface->surface()->output) {
        output = surface->surface()->output;
    } else {
        output = ctx->getCurrentOutput();
    }

    if (width == WINDOW_WIDTH_FILL_SCREEN) {
        width = output->width;
    }

    if (height == WINDOW_HEIGHT_FILL_SCREEN) {
        height = output->height;
    }

    Window *win = new Window(ctx, title, width, height);

    surface->setInputListener([](struct input_event *event, void *data) {
        Window *window = (Window *)data;

        switch (event->type) {
        case EVENT_MOUSE:
            window->onMouseEvent(event);
            break;
        case EVENT_KEYBOARD:
            window->onKeyboardEvent(event);
        }
    }, (void *)win);
    surface->setEventListener(win);

    surface->create();
    surface->commit();

    win->m_surface = surface;
    win->egl_window = (struct wl_egl_window *)ctx->createNativeWindow(surface->surface(), width, height);
    win->egl_surface = ctx->createEGLSurface(win->egl_window);

    return win;
}

void Window::onMouseEvent(struct input_event *event)
{
    struct input_event_mouse *mouseEvent = &event->mouse;

    if (m_mouseListener == nullptr) {
        return;
    }

    switch (mouseEvent->type) {
    case MOUSE_MOVE:
        m_mouseListener->onMouseMove(mouseEvent->x, mouseEvent->y);
        break;
    case MOUSE_BUTTON:
        m_mouseListener->onMouseButton(mouseEvent->button, mouseEvent->state);
        break;
    case MOUSE_ENTER:
        m_mouseListener->onMouseEnter(mouseEvent->x, mouseEvent->y);
        break;
    case MOUSE_LEAVE:
        m_mouseListener->onMouseLeave();
        break;
    }
}

void Window::onKeyboardEvent(struct input_event *event)
{
    struct input_event_keyboard *keyboardEvent = &event->keyboard;

    if (m_keyboardListener == nullptr) {
        return;
    }

    switch (keyboardEvent->type) {
    case KEYBOARD_KEY:
        m_keyboardListener->onKeyboardKey(keyboardEvent->keycode,
                keyboardEvent->keyval, keyboardEvent->state, keyboardEvent->mods);
        break;
    case KEYBOARD_ENTER:
        m_keyboardListener->onKeyboardEnter();
        break;
    case KEYBOARD_LEAVE:
        m_keyboardListener->onKeyboardLeave();
        break;
    }
}

#define invoke_or_return \
    if (m_windowEventListener == nullptr) { \
        return; \
    } \
    m_windowEventListener
void Window::onEvent(EventPtr event)
{
    switch (event->type) {
    case WINDOW_CLOSE:
        invoke_or_return->onWindowClose();
        break;
    case WINDOW_RESIZE: {
            auto e = event->cast<ResizeEvent>();
            m_width = e->width;
            m_height = e->height;

            invoke_or_return->onWindowResize(e->width, e->height);
            swapBuffer();
        } break;
    case WINDOW_MIMIMUM:
        invoke_or_return->onWindowMinimum();
        break;
    case WINDOW_MAXIMUM:
        invoke_or_return->onWindowMaximum();
        break;
    case WINDOW_HIDDEN:
        invoke_or_return->onWindowHidden();
        break;
    case WINDOW_SHOW:
        invoke_or_return->onWindowShow();
        break;
    case WINDOW_FRAME: {
            auto e = event->cast<FrameEvent>();
            draw(e->time);
        } break;
    default:
        printf("Unknown window event\n");
        break;
    }
}

void Window::setMouseListener(MouseListener *listener)
{
    m_mouseListener = listener;
}

void Window::setKeyboardListener(KeyboardListener *listener)
{
    m_keyboardListener = listener;
}

void Window::setWindowEventListener(WindowEventListener *listener)
{
    m_windowEventListener = listener;
}

void Window::setDrawEventListener(DrawEventListener *listener)
{
    m_drawEventListener = listener;
}

void Window::draw(uint32_t time)
{
    printf("[Window] new frame\n");
    if (m_drawEventListener) {
        makeCurrent();
        m_drawEventListener->onDraw(this, time);
        clearCurrent();
    }
}

std::string Window::getKeyUTF8(uint32_t keycode)
{
    return m_ctx->getKeyUTF8(keycode);
}

void Window::update()
{
    m_surface->damage();
}

}

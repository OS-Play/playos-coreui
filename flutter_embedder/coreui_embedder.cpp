#include "coreui_embedder.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

#include <EGL/egl.h>
#include <glad/glad.h>

#include "coreui_compositor.h"
#include "coreui/application.h"

#include <flutter_texture_registrar.h>


struct CoreuiFlutterState {
    FLUTTER_API_SYMBOL(FlutterEngine) engine;
    std::shared_ptr<playos::Window> window;
    playos::flutter::Embedder *embedder;

    std::unique_ptr<playos::flutter::Compositor> compositor;
    std::unique_ptr<FlutterDesktopMessenger> messenger;
    std::unique_ptr<::flutter::IncomingMessageDispatcher> messageDispatcher;
    std::unique_ptr<FlutterDesktopPluginRegistrar> pluginRegistrar;
    std::unique_ptr<::flutter::PluginRegistrar> internal_pluginRegistrar;
    std::unique_ptr<playos::flutter::KeyEvent> keyEvent;
    std::unique_ptr<playos::flutter::TextInput> textInput;
};

struct FlutterDesktopMessenger {
    CoreuiFlutterState *state;
};

struct FlutterDesktopPluginRegistrar {
    CoreuiFlutterState *state;
};

namespace playos {
namespace flutter {

Embedder::Embedder():
    isDown(false), x(0), y(0),
    m_state(std::make_unique<CoreuiFlutterState>())
{
    m_state->embedder = this;
    m_state->pluginRegistrar = std::make_unique<FlutterDesktopPluginRegistrar>();
    m_state->messenger = std::make_unique<FlutterDesktopMessenger>();

    m_state->pluginRegistrar->state = m_state.get();
    m_state->messenger->state = m_state.get();

    m_state->internal_pluginRegistrar = 
            std::make_unique<::flutter::PluginRegistrar>(m_state->pluginRegistrar.get());
    m_state->messageDispatcher = 
            std::make_unique<::flutter::IncomingMessageDispatcher>(m_state->messenger.get());
}

Embedder::~Embedder()
{
}

std::unique_ptr<Embedder> Embedder::create()
{
    return std::make_unique<Embedder>();
}

EventLoop *Embedder::eventLoop()
{
    return Application::instance()->eventLoop();
}

void Embedder::cursorPositionCallbackAtPhase(FlutterPointerPhase phase,
                                    double x,
                                    double y,
                                    int64_t buttons)
{
    FlutterPointerEvent event = {};
    event.struct_size = sizeof(event);
    event.phase = phase;
    event.x = x;
    event.y = y;
    event.timestamp =
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    if (buttons != -1) {
        event.buttons = buttons;
    }

    FlutterEngineSendPointerEvent(m_state->engine, &event, 1);
}

FlutterRendererConfig Embedder::getRendererConfig()
{
    FlutterRendererConfig config = {};

    config.type = kOpenGL;
    config.open_gl.struct_size = sizeof(config.open_gl);
    config.open_gl.make_current = [](void* userdata) -> bool {
        CoreuiFlutterState *state = (static_cast<CoreuiFlutterState *>(userdata));
        state->window->makeCurrent();
        return true;
    };
    config.open_gl.clear_current = [](void* userdata) -> bool {
        CoreuiFlutterState *state = (static_cast<CoreuiFlutterState *>(userdata));
        state->window->clearCurrent();
        return true;
    };
    config.open_gl.present = [](void* userdata) -> bool {
        CoreuiFlutterState *state = (static_cast<CoreuiFlutterState *>(userdata));
        state->window->swapBuffer();
        return true;
    };
    config.open_gl.fbo_callback = [](void*) -> uint32_t {
        return 0;  // FBO0
    };
    config.open_gl.make_resource_current = [](void *userdata) -> bool {
        CoreuiFlutterState *state = (static_cast<CoreuiFlutterState *>(userdata));
        state->window->makeResourceCurrent();

        return true;
    };
    config.open_gl.gl_proc_resolver = [](void *userdata, const char* name) -> void* {
        CoreuiFlutterState *state = (static_cast<CoreuiFlutterState *>(userdata));

        return state->window->getProcAddress(name);
    };

    return config;
}

FlutterTaskRunnerDescription Embedder::getPlatformTaskRunners()
{
    FlutterTaskRunnerDescription platform_task_runner = {};
    platform_task_runner.struct_size = sizeof(FlutterTaskRunnerDescription);
    platform_task_runner.user_data = m_state.get();
    platform_task_runner.runs_task_on_current_thread_callback = [](void* state) -> bool {
        return reinterpret_cast<CoreuiFlutterState*>(state)
                ->embedder->eventLoop()->runOnCurrentThread();
    };
    platform_task_runner.post_task_callback =
            [](FlutterTask task, uint64_t target_time_nanos, void* state) -> void {

        auto t = new playos::Task([task, state](playos::Task *t, int events) {
            auto _state = reinterpret_cast<CoreuiFlutterState *>(state);
            if (FlutterEngineRunTask(_state->engine, &task) != kSuccess) {
                std::cerr << "Could not post an engine task." << std::endl;
            }
            delete t;
        });

        reinterpret_cast<CoreuiFlutterState*>(state)->embedder->eventLoop()->post(t);
    };

    return platform_task_runner;
}

void Embedder::onMouseMove(int x, int y)
{
    this->x = x;
    this->y = y;
}

void Embedder::onMouseButton(int button, int state)
{
    if (state == 1) {
        isDown = true;
        cursorPositionCallbackAtPhase(FlutterPointerPhase::kDown, x, y, kFlutterPointerButtonMousePrimary);
        cursorPositionCallbackAtPhase(FlutterPointerPhase::kMove, x, y);
    } else {
        isDown = false;
        cursorPositionCallbackAtPhase(FlutterPointerPhase::kUp, x, y, kFlutterPointerButtonMousePrimary);
    }
}

void Embedder::onMouseEnter(int x, int y)
{
    this->x = x;
    this->y = y;
    isDown = false;
    cursorPositionCallbackAtPhase(FlutterPointerPhase::kAdd, x, y);
}

void Embedder::onMouseLeave()
{
    isDown = false;
    cursorPositionCallbackAtPhase(FlutterPointerPhase::kRemove, 0, 0);
}

void Embedder::onWindowClose()
{

}

void Embedder::onWindowResize(int width, int height)
{
    setFlutterWindowSize(width, height);
}

void Embedder::onWindowMaximum()
{

}

void Embedder::onWindowMinimum()
{

}

void Embedder::onWindowShow()
{

}

void Embedder::onWindowHidden()
{

}

void Embedder::onKeyboardKey(uint32_t keycode, uint32_t keyval, int state, int mods)
{
    // printf(">>>>>>:%X, %x, %d\n", keycode, keyval, state);
    // FlutterKeyEvent event = {
    //     .struct_size = sizeof(FlutterKeyEvent),
    //     .timestamp = (double)FlutterEngineGetCurrentTime(),
    //     .type = (state == 1 ? kFlutterKeyEventTypeDown : kFlutterKeyEventTypeUp),
    //     .physical = uint64_t(0x00070007),
    //     .logical = uint64_t(0x00000000064),
    //     // .physical = coreui_getPhysicalKeyboardKey(key),
    //     // .logical = coreui_getLogicalKeyboardKey(key),
    //     .character = nullptr,
    //     .synthesized = false,
    // };

    // FlutterEngineResult ret = 
    //     FlutterEngineSendKeyEvent(reinterpret_cast<FlutterEngine>(window->priv()),
    //         &event, [](bool handled, void *) {
    //             printf(">>>>>>>>>>handle: %s\n", handled ? "yes" : "no");
    //         }, nullptr);

    if (m_state->keyEvent) {
        m_state->keyEvent->sendKey(keycode, keyval, state, mods);
    }

    if (m_state->textInput) {
        m_state->textInput->sendKey(keycode, keyval, state, mods);
    }
}

bool Embedder::runFlutter(std::shared_ptr<Window> window,
                const std::string& assets_path,
                const std::string& icudtl_path,
                int argc, const char **argv)
{
    m_state->window = window;
    window->createResourceContext();
    window->setMouseListener(this);
    window->setKeyboardListener(this);
    m_state->compositor = std::make_unique<playos::flutter::Compositor>(window);
    auto compositor = m_state->compositor->getFlutterCompositor();

    FlutterRendererConfig config = getRendererConfig();

    FlutterCustomTaskRunners task_runners = {};
    auto platform_task_runner = getPlatformTaskRunners();
    task_runners.struct_size = sizeof(FlutterCustomTaskRunners);
    task_runners.platform_task_runner = &platform_task_runner;
    task_runners.render_task_runner = &platform_task_runner;

    FlutterProjectArgs args = {
        .struct_size = sizeof(FlutterProjectArgs),
        .assets_path = assets_path.c_str(),
        .icu_data_path =
            icudtl_path.c_str(),  // Find this in your bin/cache directory.
        .command_line_argc = argc,
        .command_line_argv = argv,
        .platform_message_callback = [](const FlutterPlatformMessage* msg, void* data) {
            auto state = reinterpret_cast<CoreuiFlutterState*>(data);

            state->embedder->platformMessageCallback(msg);
        },
        // .vsync_callback = [](void* data, intptr_t baton) {
        //     Application::instance()->eventLoop()->post(new Task([data, baton](Task *task, int) {
        //         auto state = reinterpret_cast<CoreuiFlutterState*>(data);

        //         FlutterEngineOnVsync(state->engine, baton, FlutterEngineGetCurrentTime(), 0);
        //     }));
        // },
        .custom_task_runners = &task_runners,
        .compositor = &compositor,
        .log_message_callback = [](const char* tag, const char* msg, void *data) {
            printf("[%s]: %s\n", tag, msg);
        },
    };

    FlutterEngineResult result =
        FlutterEngineRun(FLUTTER_ENGINE_VERSION, &config,  // renderer
                        &args, m_state.get(), &m_state->engine);
    if (result != kSuccess || m_state->engine == nullptr) {
        std::cout << "Could not run the Flutter Engine." << std::endl;
        return false;
    }

    auto internal_plugin_messenger = m_state->internal_pluginRegistrar->messenger();
    m_state->keyEvent = std::make_unique<KeyEvent>(internal_plugin_messenger, window);
    m_state->textInput = std::make_unique<TextInput>(internal_plugin_messenger, window);

    setFlutterWindowSize(window->width(), window->height());

    return true;
}

void Embedder::setFlutterWindowSize(int width, int height)
{
    FlutterWindowMetricsEvent event = {};
    event.struct_size = sizeof(event);
    event.width = width;
    event.height = height;
    event.pixel_ratio = 1.0;
    FlutterEngineSendWindowMetricsEvent(m_state->engine, &event);
}

void Embedder::platformMessageCallback(const FlutterPlatformMessage* msg)
{
    if (msg->struct_size != sizeof(FlutterPlatformMessage)) {
        std::cerr << "Invalid message size received. Expected: "
                << sizeof(FlutterPlatformMessage) << " but received "
                << msg->struct_size << std::endl;
        return;
    }

    FlutterDesktopMessage message = {};
    message.struct_size = sizeof(message);
    message.channel = msg->channel;
    message.message = msg->message;
    message.message_size = msg->message_size;
    message.response_handle = msg->response_handle;

    m_state->messageDispatcher->HandleMessage(
        message,
        [] { },
        [] { });
}

bool Embedder::sendPlatformMessage(
        const char* channel,
        const uint8_t* message,
        const size_t message_size,
        const FlutterDesktopBinaryReply reply,
        void* user_data)
{
    FlutterPlatformMessage msg;

    msg.struct_size = sizeof(FlutterPlatformMessage);
    msg.channel = channel;
    msg.message = message;
    msg.message_size = message_size;

    FlutterPlatformMessageResponseHandle *response_handle = nullptr;
    if (reply) {
        struct _replyWrapper {
            ::flutter::BinaryReply reply;
        };

        auto replyWrapper = new _replyWrapper();
        FlutterPlatformMessageCreateResponseHandle(m_state->engine, 
                [](const uint8_t* reply, size_t reply_size, void *userdata) {
            auto wrapper = reinterpret_cast<_replyWrapper *>(userdata);
            wrapper->reply(reply, reply_size);
            delete wrapper;
        }, (void *)replyWrapper, &response_handle);
    }
    msg.response_handle = response_handle;

    FlutterEngineResult ret = FlutterEngineSendPlatformMessage(m_state->engine, &msg);

    if (response_handle) {
        FlutterPlatformMessageReleaseResponseHandle(m_state->engine, response_handle);
    }

    return ret == kSuccess;
}

void Embedder::setMessageCallback(const char* channel,
        FlutterDesktopMessageCallback callback, void* user_data)
{
    m_state->messageDispatcher->SetMessageCallback(channel, callback, user_data);
}

}
}

// Flutter embedder implementation

bool FlutterDesktopMessengerSend(
    FlutterDesktopMessengerRef messenger,
    const char* channel,
    const uint8_t* message,
    const size_t message_size)
{
    return FlutterDesktopMessengerSendWithReply(messenger, channel,
            message, message_size, nullptr, nullptr);
}

bool FlutterDesktopMessengerSendWithReply(
    FlutterDesktopMessengerRef messenger,
    const char* channel,
    const uint8_t* message,
    const size_t message_size,
    const FlutterDesktopBinaryReply reply,
    void* user_data)
{
    return messenger->state->embedder->sendPlatformMessage(channel,
            message, message_size, reply, user_data);
}

void FlutterDesktopMessengerSendResponse(
    FlutterDesktopMessengerRef messenger,
    const FlutterDesktopMessageResponseHandle* handle,
    const uint8_t* data,
    size_t data_length)
{
    FlutterEngineSendPlatformMessageResponse(messenger->state->engine,
                                           handle, data, data_length);
}

void FlutterDesktopMessengerSetCallback(
    FlutterDesktopMessengerRef messenger,
    const char* channel,
    FlutterDesktopMessageCallback callback,
    void* user_data)
{
    messenger->state->embedder->setMessageCallback(channel, callback, user_data);
}

int64_t FlutterDesktopTextureRegistrarRegisterExternalTexture(
    FlutterDesktopTextureRegistrarRef texture_registrar,
    const FlutterDesktopTextureInfo* info)
{
    return -1;
}

bool FlutterDesktopTextureRegistrarUnregisterExternalTexture(
    FlutterDesktopTextureRegistrarRef texture_registrar,
    int64_t texture_id)
{
    return false;
}

bool FlutterDesktopTextureRegistrarMarkExternalTextureFrameAvailable(
    FlutterDesktopTextureRegistrarRef texture_registrar,
    int64_t texture_id)
{
    return false;
}

FlutterDesktopMessengerRef
FlutterDesktopPluginRegistrarGetMessenger(
    FlutterDesktopPluginRegistrarRef registrar)
{
    return registrar->state->messenger.get();
}

// Returns the texture registrar associated with this registrar.
FlutterDesktopTextureRegistrarRef
FlutterDesktopRegistrarGetTextureRegistrar(
    FlutterDesktopPluginRegistrarRef registrar)
{
    return nullptr;
}

// Registers a callback to be called when the plugin registrar is destroyed.
void FlutterDesktopPluginRegistrarSetDestructionHandler(
    FlutterDesktopPluginRegistrarRef registrar,
    FlutterDesktopOnPluginRegistrarDestroyed callback)
{
}
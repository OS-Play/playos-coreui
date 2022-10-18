#ifndef __PLAYOS_COREUI_EMBEDDER_H__
#define __PLAYOS_COREUI_EMBEDDER_H__

#include <memory>
#include <string>
#include <unordered_map>
#include <queue>
#include <mutex>

#include <embedder.h>
#include <flutter_messenger.h>
#include <flutter/plugin_registrar.h>
#include "flutter/shell/platform/common/incoming_message_dispatcher.h"

#include "coreui/window.h"
#include "plugins/coreui_key_event.h"
#include "plugins/coreui_text_input.h"


struct CoreuiFlutterState;

namespace playos {

class EventLoop;

namespace flutter {

class Embedder: public MouseListener, public WindowEventListener,
        public KeyboardListener {
private:
    Embedder();

    void cursorPositionCallbackAtPhase(FlutterPointerPhase phase,
                                       double x,
                                       double y,
                                       int64_t buttons = -1);

    FlutterRendererConfig getRendererConfig();
    FlutterTaskRunnerDescription getPlatformTaskRunners();

public:
    static std::unique_ptr<Embedder> create();
    ~Embedder();

    EventLoop *eventLoop();

    bool runFlutter(std::shared_ptr<Window> window, const std::string& project_path,
                    const std::string& icudtl_path,
                    int argc, const char **argv);

    void setFlutterWindowSize(int width, int height);
    void platformMessageCallback(const FlutterPlatformMessage* msg);

    bool sendPlatformMessage(
            const char* channel,
            const uint8_t* message,
            const size_t message_size,
            const FlutterDesktopBinaryReply reply,
            void* user_data);
    void setMessageCallback(const char* channel,
        FlutterDesktopMessageCallback callback, void* user_data);

public:
    void onMouseMove(int x, int y);
    void onMouseButton(int button, int state);
    void onMouseEnter(int x, int y);
    void onMouseLeave();

public:
    void onWindowClose();
    void onWindowResize(int width, int height);
    void onWindowMaximum();
    void onWindowMinimum();
    void onWindowShow();
    void onWindowHidden();

public:
    void onKeyboardKey(uint32_t keycode, uint32_t keyval, int state, int mods);

private:
    bool isDown;
    int x, y;
    std::unique_ptr<CoreuiFlutterState> m_state;
    std::mutex m_vsyncMux;
    std::queue<intptr_t> m_vsyncQueue;

    friend std::unique_ptr<Embedder> std::make_unique<Embedder>();
};

}
}

#endif

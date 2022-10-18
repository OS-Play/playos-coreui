#ifndef __COREUI_KEY_EVENT_H__
#define __COREUI_KEY_EVENT_H__

#include <memory>

#include <embedder.h>
#include <flutter/basic_message_channel.h>
// Header from libX11 defined it, may be include in wlroots, undef here
#ifdef Bool
#undef Bool
#endif
#include "rapidjson/document.h"

#include "coreui/window.h"



namespace playos {
namespace flutter {

class KeyEvent {

public:
    KeyEvent(::flutter::BinaryMessenger *messenger, std::shared_ptr<Window> window);
    ~KeyEvent();

    void sendKey(int key, int scancode, int state, int mods);

protected:
    std::unique_ptr<::flutter::BasicMessageChannel<rapidjson::Document>> m_channel;
    std::shared_ptr<Window> m_window;
};

}
}

#endif

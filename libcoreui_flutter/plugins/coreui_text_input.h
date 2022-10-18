#ifndef __COREUI_TEXT_INPUT_H__
#define __COREUI_TEXT_INPUT_H__

#include <memory>

#include <embedder.h>

#include "rapidjson/document.h"

#include <flutter/binary_messenger.h>
// Header from libX11 defined it, may be include in wlroots, undef here
#ifdef Success
#undef Success
#endif
#include <flutter/method_channel.h>


namespace playos {

class Window;

namespace flutter {

class InputModel;

class TextInput {

public:
    TextInput(::flutter::BinaryMessenger *messenger, std::shared_ptr<Window> window);
    ~TextInput();

    void sendKey(int key, int scancode, int state, int mods);

    void handleMethodCall(
        const ::flutter::MethodCall<rapidjson::Document>& method_call,
        std::unique_ptr<::flutter::MethodResult<rapidjson::Document>> result);

private:
    void updateState();
    
private:
    std::unique_ptr<::flutter::MethodChannel<rapidjson::Document>> m_channel;
    std::unique_ptr<InputModel> m_activeModel;
    std::shared_ptr<Window> m_window;

    // The active client id.
    int m_clientId = 0;

    // Keyboard type of the client. See available options:
    // https://api.flutter.dev/flutter/services/TextInputType-class.html
    std::string m_inputType;

    // An action requested by the user on the input client. See available options:
    // https://api.flutter.dev/flutter/services/TextInputAction-class.html
    std::string m_inputAction;
};

}
}


#endif

#ifndef __COREUI_CHANNEL_H__
#define __COREUI_CHANNEL_H__

#include "coreui_messenger.h"

#include "rapidjson/document.h"

#include <functional>
#include <string>
#include <memory>

namespace playos {
namespace flutter {

class MessageChannel {
public:
    MessageChannel(const std::string &channel, Messenger *messenger);
    ~MessageChannel();

    FlutterEngineResult send(rapidjson::Document &event);

protected:
    std::string m_channelName;
    Messenger *m_messenger;
};

class MethodCall {

};

class MethodResult {

};

class MethodChannel: public MessageChannel {
public:
    using MethodHandler = std::function<void(const MethodCall& call,
          std::unique_ptr<MethodResult> result)>;
public:
    MethodChannel(const std::string &channel, Messenger *messenger);
    ~MethodChannel();

    void setMethodHandler(MethodHandler handler) {
        m_methodHandler = handler;
    }

private:
    MethodHandler m_methodHandler;
};

}
}

#endif

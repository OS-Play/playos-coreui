#ifndef __COREUI_MESSAGER_H__
#define __COREUI_MESSAGER_H__

#include <embedder.h>

#include <flutter/binary_messenger.h>


namespace playos {
namespace flutter {


class Messenger: public ::flutter::BinaryMessenger {
public:
    Messenger(FLUTTER_API_SYMBOL(FlutterEngine) engine);
    ~Messenger();

    void Send(const std::string& channel,
                    const uint8_t* message,
                    size_t message_size,
                    ::flutter::BinaryReply reply = nullptr) const;

    void SetMessageHandler(const std::string& channel,
                                 ::flutter::BinaryMessageHandler handler);
private:
    FLUTTER_API_SYMBOL(FlutterEngine) m_engine;
};

}
}

#endif

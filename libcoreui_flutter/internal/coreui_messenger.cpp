#include "coreui_messenger.h"

#include <stdio.h>
#include <string>


namespace playos {
namespace flutter {

Messenger::Messenger(FLUTTER_API_SYMBOL(FlutterEngine) engine):
        m_engine(engine)
{
}

Messenger::~Messenger()
{
}

void Messenger::Send(const std::string& channel,
                    const uint8_t* message,
                    size_t message_size,
                    ::flutter::BinaryReply reply) const
{
    FlutterPlatformMessage msg;

    msg.struct_size = sizeof(FlutterPlatformMessage);
    msg.channel = channel.c_str();
    msg.message = message;
    msg.message_size = message_size;

    FlutterPlatformMessageResponseHandle *response_handle = nullptr;
    if (reply) {
        struct _replyWrapper {
            ::flutter::BinaryReply reply;
        };

        auto replyWrapper = new _replyWrapper();
        FlutterPlatformMessageCreateResponseHandle(m_engine, 
                [](const uint8_t* reply, size_t reply_size, void *userdata) {
            auto wrapper = reinterpret_cast<_replyWrapper *>(userdata);
            wrapper->reply(reply, reply_size);
            delete wrapper;
        }, (void *)replyWrapper, &response_handle);
    }
    msg.response_handle = response_handle;

    FlutterEngineSendPlatformMessage(m_engine, &msg);

    if (response_handle) {
        FlutterPlatformMessageReleaseResponseHandle(m_engine, response_handle);
    }
}

void Messenger::SetMessageHandler(const std::string& channel,
                                ::flutter::BinaryMessageHandler handler)
{
    
}

}
}

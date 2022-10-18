#include "coreui_channel.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


namespace playos {
namespace flutter {

MessageChannel::MessageChannel(const std::string &channel, Messenger *messenger):
        m_channelName(channel), m_messenger(messenger)
{

}

MessageChannel::~MessageChannel()
{
}

FlutterEngineResult MessageChannel::send(rapidjson::Document &event)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    event.Accept(writer);

    auto st = buffer.GetString();
    m_messenger->Send(m_channelName.c_str(), (uint8_t *) st, strlen(st));

    return kSuccess;
}

MethodChannel::MethodChannel(const std::string &channel, Messenger *messenger):
        MessageChannel(channel, messenger)
{

}

MethodChannel::~MethodChannel()
{

}

}
}
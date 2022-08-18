#ifndef __COREUI_JSON_MESSAGE_CODEC_H__
#define __COREUI_JSON_MESSAGE_CODEC_H__

#include <flutter/message_codec.h>

#include "rapidjson/document.h"


namespace playos {
namespace flutter {

class JsonMessageCodec: public ::flutter::MessageCodec<rapidjson::Document> {
public:
    static JsonMessageCodec &instance();

protected:
    std::unique_ptr<rapidjson::Document> DecodeMessageInternal(
            const uint8_t* binary_message,
            const size_t message_size) const;

    std::unique_ptr<std::vector<uint8_t>> EncodeMessageInternal(
        const rapidjson::Document& message) const;
};

}
}

#endif

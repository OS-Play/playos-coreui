#ifndef __JSON_MEHOD_CODEC_H__
#define __JSON_MEHOD_CODEC_H__

#include "rapidjson/document.h"

#include <flutter/method_call.h>
#include <flutter/method_codec.h>


namespace playos {
namespace flutter {

class JsonMethodCodec: public ::flutter::MethodCodec<rapidjson::Document> {
public:
    static JsonMethodCodec &instance();

protected:
    // Implementation of the public interface, to be provided by subclasses.
    std::unique_ptr<::flutter::MethodCall<rapidjson::Document>> DecodeMethodCallInternal(
        const uint8_t* message,
        size_t message_size) const;

    // Implementation of the public interface, to be provided by subclasses.
    std::unique_ptr<std::vector<uint8_t>> EncodeMethodCallInternal(
        const ::flutter::MethodCall<rapidjson::Document>& method_call) const;

    // Implementation of the public interface, to be provided by subclasses.
    std::unique_ptr<std::vector<uint8_t>> EncodeSuccessEnvelopeInternal(
        const rapidjson::Document* result) const;

    // Implementation of the public interface, to be provided by subclasses.
    std::unique_ptr<std::vector<uint8_t>> EncodeErrorEnvelopeInternal(
        const std::string& error_code,
        const std::string& error_message,
        const rapidjson::Document* error_details) const;

    // Implementation of the public interface, to be provided by subclasses.
    bool DecodeAndProcessResponseEnvelopeInternal(
        const uint8_t* response,
        size_t response_size,
        ::flutter::MethodResult<rapidjson::Document>* result) const;
};

}
}

#endif

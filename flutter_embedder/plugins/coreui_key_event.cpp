#include "coreui_key_event.h"

#include "../internal/json_message_codec.h"

#include <memory>

static constexpr char kChannelName[] = "flutter/keyevent";

static constexpr char kKeyCodeKey[] = "keyCode";
static constexpr char kKeyMapKey[] = "keymap";
static constexpr char kScanCodeKey[] = "scanCode";
static constexpr char kModifiersKey[] = "modifiers";
static constexpr char kTypeKey[] = "type";
static constexpr char kToolkitKey[] = "toolkit";
static constexpr char kUnicodeScalarValues[] = "unicodeScalarValues";

static constexpr char kLinuxKeyMap[] = "linux";
// static constexpr char kCoreuiKey[] = "coreui";
static constexpr char kCoreuiKey[] = "gtk";

static constexpr char kKeyUp[] = "keyup";
static constexpr char kKeyDown[] = "keydown";

const uint64_t c_ValueMask = 0x000ffffffff;
const uint64_t c_UnicodePlane = 0x00000000000;
const uint64_t c_CoreuiPlane = 0x01500000000;

namespace playos {
namespace flutter {

KeyEvent::KeyEvent(::flutter::BinaryMessenger *messenger, std::shared_ptr<Window> window):
        m_window(window),
        m_channel(
          std::make_unique<::flutter::BasicMessageChannel<rapidjson::Document>>(
              messenger,
              kChannelName,
              &JsonMessageCodec::instance()))
{
}

KeyEvent::~KeyEvent()
{
}

void KeyEvent::sendKey(int scancode, int keyval, int state, int mods)
{
    rapidjson::Document event(rapidjson::kObjectType);
    auto& allocator = event.GetAllocator();
    event.AddMember(kKeyCodeKey, keyval, allocator);
    event.AddMember(kKeyMapKey, kLinuxKeyMap, allocator);
    event.AddMember(kScanCodeKey, scancode, allocator);
    event.AddMember(kModifiersKey, mods, allocator);
    event.AddMember(kToolkitKey, kCoreuiKey, allocator);

    std::string u = m_window->getKeyUTF8(scancode);
    if (!u.empty()) {
        event.AddMember(kUnicodeScalarValues, *((uint16_t *)u.c_str()), allocator);
    }

    switch (state) {
    case 1:
        event.AddMember(kTypeKey, kKeyDown, allocator);
        break;
    case 0:
        event.AddMember(kTypeKey, kKeyUp, allocator);
        break;
    default:
        printf("Unknown key event action: %d\n", state);
        return;
    }

    m_channel->Send(event);
}

}
}

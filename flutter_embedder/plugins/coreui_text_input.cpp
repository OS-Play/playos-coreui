#include "coreui_text_input.h"

#include "../internal/json_method_codec.h"
#include "coreui/window.h"
#include <xkbcommon/xkbcommon-keysyms.h>

static constexpr char kSetEditingStateMethod[] = "TextInput.setEditingState";
static constexpr char kClearClientMethod[] = "TextInput.clearClient";
static constexpr char kSetClientMethod[] = "TextInput.setClient";
static constexpr char kShowMethod[] = "TextInput.show";
static constexpr char kHideMethod[] = "TextInput.hide";

static constexpr char kMultilineInputType[] = "TextInputType.multiline";

static constexpr char kUpdateEditingStateMethod[] =
    "TextInputClient.updateEditingState";
static constexpr char kPerformActionMethod[] = "TextInputClient.performAction";

static constexpr char kTextInputAction[] = "inputAction";
static constexpr char kTextInputType[] = "inputType";
static constexpr char kTextInputTypeName[] = "name";
static constexpr char kComposingBaseKey[] = "composingBase";
static constexpr char kComposingExtentKey[] = "composingExtent";
static constexpr char kSelectionAffinityKey[] = "selectionAffinity";
static constexpr char kAffinityDownstream[] = "TextAffinity.downstream";
static constexpr char kSelectionBaseKey[] = "selectionBase";
static constexpr char kSelectionExtentKey[] = "selectionExtent";
static constexpr char kSelectionIsDirectionalKey[] = "selectionIsDirectional";
static constexpr char kTextKey[] = "text";

static constexpr char kChannelName[] = "flutter/textinput";

static constexpr char kBadArgumentError[] = "Bad Arguments";
static constexpr char kInternalConsistencyError[] =
    "Internal Consistency Error";

namespace playos {
namespace flutter {

class InputModel {
public:
    int cursorStart() {
        return m_cursorStart;
    }

    int cursorEnd() {
        return m_cursorEnd;
    }

    void cursorMoveLeft() {
        m_cursorStart = m_cursorEnd = (m_cursorStart - 1);
        if (m_cursorStart < 0) {
            m_cursorStart = m_cursorEnd = 0;
        }
    }

    void cursorMoveRight() {
        m_cursorStart = m_cursorEnd = (m_cursorEnd + 1);
        if (m_cursorStart > m_text.length()) {
            m_cursorStart = m_cursorEnd = m_text.length();
        }
    }

    void cursorMoveStart() {
        m_cursorStart = m_cursorEnd = 0;
    }

    void cursorMoveEnd() {
        m_cursorStart = m_cursorEnd = m_text.length();
    }

    void append(const char *data, size_t len) {
        append(std::string(data, len));
    }

    void append(const std::string &str) {
        if (m_cursorStart != m_cursorEnd) {
            m_text.erase(m_text.begin() + m_cursorStart, m_text.begin() + (m_cursorEnd + 1));
        }

        m_text.insert(m_text.begin() + m_cursorStart, str.begin(), str.end());

        m_cursorStart = m_cursorEnd = (m_cursorEnd + str.length());
    }

    void remove() {
        if (m_cursorStart == m_cursorEnd && m_cursorEnd == 0) {
            return;
        }

        auto s = m_text.begin();
        auto e = m_text.begin();
        if (m_cursorStart == m_cursorEnd) {
            s += m_cursorStart - 1;
            e += m_cursorEnd;

            m_cursorStart = m_cursorEnd = m_cursorStart - 1;
        } else {
            s += m_cursorStart;
            e += m_cursorEnd + 1;

            m_cursorEnd = m_cursorStart;
        }

        if (s > m_text.end()) {
            s = m_text.end() - 1;
        } else if (s < m_text.begin()) {
            s = m_text.begin();
        }

        if (e > m_text.end()) {
            e = m_text.end();
        } else if (e < m_text.begin()) {
            e = m_text.begin();
        }

        m_text.erase(s, e);
    }

    void delete_() {
        if (m_cursorStart == m_cursorEnd && m_cursorEnd == m_text.length()) {
            return;
        }

        auto s = m_text.begin();
        auto e = m_text.begin();

        s += m_cursorStart;
        e += m_cursorEnd + 1;

        if (s > m_text.end()) {
            s = m_text.end() - 1;
        } else if (s < m_text.begin()) {
            s = m_text.begin();
        }

        if (e > m_text.end()) {
            e = m_text.end();
        } else if (e < m_text.begin()) {
            e = m_text.begin();
        }

        m_text.erase(s, e);
        m_cursorEnd = m_cursorStart;
    }

    std::string text() {
        return m_text;
    }

private:
    int m_cursorStart;
    int m_cursorEnd;
    std::string m_text;

    friend class TextInput;
};

TextInput::TextInput(::flutter::BinaryMessenger *messenger, std::shared_ptr<Window> window):
        m_channel(std::make_unique<::flutter::MethodChannel<rapidjson::Document>>(
            messenger,
            kChannelName,
            &JsonMethodCodec::instance())),
        m_activeModel(nullptr),
        m_window(window)
{
      m_channel->SetMethodCallHandler(
            [this](
                const ::flutter::MethodCall<rapidjson::Document>& call,
                std::unique_ptr<::flutter::MethodResult<rapidjson::Document>> result) {
                handleMethodCall(call, std::move(result));
            });
}

TextInput::~TextInput()
{
}

void TextInput::sendKey(int scancode, int keyval, int state, int mods)
{
    if (!m_activeModel || state == 1) {
        return;
    }

    switch (keyval) {
    case XKB_KEY_BackSpace:
        // m_activeModel->remove();
        break;
    case XKB_KEY_Delete:
        // m_activeModel->delete_();
        break;
    case XKB_KEY_Left:
        // m_activeModel->cursorMoveLeft();
        break;
    case XKB_KEY_Right:
        // m_activeModel->cursorMoveRight();
        break;
    case XKB_KEY_Down:
    case XKB_KEY_End:
        // m_activeModel->cursorMoveEnd();
        break;
    case XKB_KEY_Up:
    case XKB_KEY_Home:
    case XKB_KEY_Begin:
        // m_activeModel->cursorMoveStart();
        break;
    case XKB_KEY_Return: {
            auto args = std::make_unique<rapidjson::Document>(rapidjson::kArrayType);
            auto& allocator = args->GetAllocator();
            args->PushBack(m_clientId, allocator);
            args->PushBack(rapidjson::Value(m_inputAction, allocator).Move(), allocator);

            m_channel->InvokeMethod(kPerformActionMethod, std::move(args));
        } return;
    default: {
            std::string u = m_window->getKeyUTF8(scancode);
            if (u.length() > 0) {
                if (u.length() == 1 && !isprint(u[0]))
                    return;
                m_activeModel->append(u);
            }
        }
        break;
    }

    updateState();
}

void TextInput::handleMethodCall(
        const ::flutter::MethodCall<rapidjson::Document>& method_call,
        std::unique_ptr<::flutter::MethodResult<rapidjson::Document>> result)
{
    auto &method = method_call.method_name();
    if (method == kSetEditingStateMethod) {
        if (!method_call.arguments() || method_call.arguments()->IsNull()) {
            result->Error(kBadArgumentError, "Method invoked without args");
            return;
        }
        const rapidjson::Document& args = *method_call.arguments();

        if (m_activeModel == nullptr) {
            result->Error(
                kInternalConsistencyError,
                "Set editing state has been invoked, but no client is set.");
            return;
        }
        auto text = args.FindMember(kTextKey);
        if (text == args.MemberEnd() || text->value.IsNull()) {
            result->Error(kBadArgumentError,
                        "Set editing state has been invoked, but without text.");
            return;
        }
        auto selection_base = args.FindMember(kSelectionBaseKey);
        auto selection_extent = args.FindMember(kSelectionExtentKey);
        if (selection_base == args.MemberEnd() || selection_base->value.IsNull() ||
            selection_extent == args.MemberEnd() ||
            selection_extent->value.IsNull()) {
            result->Error(kInternalConsistencyError,
                        "Selection base/extent values invalid.");
            return;
        }
        // Flutter uses -1/-1 for invalid; translate that to 0/0 for the model.
        int base = selection_base->value.GetInt();
        int extent = selection_extent->value.GetInt();
        if (base == -1 && extent == -1) {
            base = extent = 0;
        }
        m_activeModel->m_text = text->value.GetString();
        m_activeModel->m_cursorStart = base;
        m_activeModel->m_cursorEnd = extent;
    } else if (method == kClearClientMethod) {
        m_activeModel = nullptr;
    } else if (method == kSetClientMethod) {
        if (!method_call.arguments() || method_call.arguments()->IsNull()) {
        result->Error(kBadArgumentError, "Method invoked without args");
        return;
        }
        const rapidjson::Document& args = *method_call.arguments();

        // TODO(awdavies): There's quite a wealth of arguments supplied with this
        // method, and they should be inspected/used.
        const rapidjson::Value& client_id_json = args[0];
        const rapidjson::Value& client_config = args[1];
        if (client_id_json.IsNull()) {
            result->Error(kBadArgumentError, "Could not set client, ID is null.");
            return;
        }
        if (client_config.IsNull()) {
            result->Error(kBadArgumentError,
                            "Could not set client, missing arguments.");
            return;
        }
        m_clientId = client_id_json.GetInt();
        m_inputAction = "";
        auto input_action_json = client_config.FindMember(kTextInputAction);
        if (input_action_json != client_config.MemberEnd() &&
                input_action_json->value.IsString()) {
            m_inputAction = input_action_json->value.GetString();
        }
        m_inputType = "";
        auto input_type_info_json = client_config.FindMember(kTextInputType);
        if (input_type_info_json != client_config.MemberEnd() &&
                input_type_info_json->value.IsObject()) {
            auto input_type_json =
                input_type_info_json->value.FindMember(kTextInputTypeName);
            if (input_type_json != input_type_info_json->value.MemberEnd() &&
                input_type_json->value.IsString()) {
                m_inputType = input_type_json->value.GetString();
            }
        }

        m_activeModel = std::make_unique<InputModel>();
    } else if (method == kShowMethod) {

    } else if (method == kHideMethod) {

    } else {
        printf("[TextInput] NotImplemented method: %s\n", method.c_str());
        result->NotImplemented();
        return;
    }

    result->Success();
}

void TextInput::updateState()
{
    auto args = std::make_unique<rapidjson::Document>(rapidjson::kArrayType);
    auto& allocator = args->GetAllocator();
    args->PushBack(m_clientId, allocator);

    rapidjson::Value editing_state(rapidjson::kObjectType);
    editing_state.AddMember(kComposingBaseKey, -1, allocator);
    editing_state.AddMember(kComposingExtentKey, -1, allocator);
    editing_state.AddMember(kSelectionAffinityKey, kAffinityDownstream,
                            allocator);
    editing_state.AddMember(kSelectionBaseKey, m_activeModel->cursorStart(), allocator);
    editing_state.AddMember(kSelectionExtentKey, m_activeModel->cursorEnd(), allocator);
    editing_state.AddMember(kSelectionIsDirectionalKey, false, allocator);
    editing_state.AddMember(
        kTextKey, rapidjson::Value(m_activeModel->text(), allocator).Move(), allocator);
    args->PushBack(editing_state, allocator);

    m_channel->InvokeMethod(kUpdateEditingStateMethod, std::move(args));
}

}
}

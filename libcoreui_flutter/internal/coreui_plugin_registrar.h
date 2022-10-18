#ifndef __COREUI_PLUGIN_REGISTRAR_H__
#define __COREUI_PLUGIN_REGISTRAR_H__

#include <memory>

#include <embedder.h>
#include "coreui_messenger.h"

namespace playos {
namespace flutter {

class PluginRegistrar {
public:
    PluginRegistrar(FLUTTER_API_SYMBOL(FlutterEngine) engine);
    ~PluginRegistrar();

    ::flutter::BinaryMessenger *messenger();

private:
    FLUTTER_API_SYMBOL(FlutterEngine) m_engine;
    std::unique_ptr<Messenger> m_messenger;
};

}
}

#endif

#include "coreui_plugin_registrar.h"

#include "../coreui_embedder.h"

namespace playos {
namespace flutter {

PluginRegistrar::PluginRegistrar(FLUTTER_API_SYMBOL(FlutterEngine) engine):
        m_engine(engine)
{
    m_messenger = std::make_unique<Messenger>(engine);
}

PluginRegistrar::~PluginRegistrar()
{
}

::flutter::BinaryMessenger *PluginRegistrar::messenger()
{
    return m_messenger.get();
}



}
}


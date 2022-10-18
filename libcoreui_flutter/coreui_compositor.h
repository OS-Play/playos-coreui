#ifndef __PLAYOS_COREUI_COMPOSITOR_H__
#define __PLAYOS_COREUI_COMPOSITOR_H__

#include <embedder.h>

#include "coreui/window.h"

namespace playos {
namespace flutter {

class Compositor {
public:
    Compositor(std::shared_ptr<Window> m_window);
    ~Compositor();

    FlutterCompositor getFlutterCompositor();

public:
    bool createBackingStore(const FlutterBackingStoreConfig* config,
            FlutterBackingStore* backing_store_out);
    bool collectBackingStore(const FlutterBackingStore* renderer);
    bool presentLayers(const FlutterLayer** layers, size_t layers_count);

private:
    std::shared_ptr<Window> m_window;
};

}
}

#endif

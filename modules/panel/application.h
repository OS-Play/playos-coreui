#ifndef __PLAYOS_PANEL_APPLICATION_H__
#define __PLAYOS_PANEL_APPLICATION_H__

#include "coreui/flutter/application.h"
#include <memory>


namespace playos {

class PanelApplication: public flutter::Application {
public:
    PanelApplication(int argc, char *argv[]);
    ~PanelApplication();

    std::shared_ptr<Window> createDefaultWindow();
};

}

#endif

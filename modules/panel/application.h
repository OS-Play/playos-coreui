#ifndef __PLAYOS_PANEL_APPLICATION_H__
#define __PLAYOS_PANEL_APPLICATION_H__

#include "coreui/flutter/application.h"
#include <memory>


namespace playos {

class PanelApplication: public flutter::Application {
public:
    PanelApplication(int argc, char *argv[], const char *appPath, const char *icudtl);
    ~PanelApplication();

    std::shared_ptr<Window> createDefaultWindow();
};

}

#endif

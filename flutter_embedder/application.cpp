#include "coreui/flutter/application.h"
#include "coreui/application.h"

#include "coreui/window.h"
#include "coreui/flutter/embedder.h"


namespace playos {
namespace flutter {

Application::Application(int argc, char **argv, const char *assetsPath, const char *icudtl):
        playos::Application(argc, argv), m_assetsPath(assetsPath), m_icudtl(icudtl)
{

}

Application::~Application()
{
}

std::shared_ptr<Window> Application::createDefaultWindow()
{
    struct coreui_output *output = context()->getCurrentOutput();

    return std::shared_ptr<Window>(createWindow("main", output->width, output->height));
}

int Application::init()
{
    int ret = playos::Application::init();

    if (ret != 0) {
        return ret;
    }

    m_window = createDefaultWindow();
    if (!RunFlutter(m_window.get(), m_assetsPath.c_str(), m_icudtl.c_str(), argc, (const char **)argv)) {
        return -1;
    }

    return 0;
}

}
}

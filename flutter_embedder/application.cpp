#include "coreui/flutter/application.h"
#include "coreui/application.h"

#include "coreui/window.h"

#include <sys/stat.h>


namespace playos {
namespace flutter {

Application::Application(int argc, char **argv, const char *assetsPath, const char *icudtl):
        playos::Application(argc, argv), m_embedder(Embedder::create()),
            m_assetsPath(assetsPath), m_icudtl(icudtl)
{
    if (argc > 1) {
        struct stat st;
        if(stat(argv[1], &st) == 0) {
            if(st.st_mode & S_IFDIR != 0) {
                m_assetsPath = std::string(argv[1]);
            }
        }
    }
}

Application::~Application()
{
}

std::shared_ptr<Window> Application::createDefaultWindow()
{
    return std::shared_ptr<Window>(createWindow("main", WINDOW_WIDTH_FILL_SCREEN, WINDOW_WIDTH_FILL_SCREEN));
}

int Application::onInit()
{
    m_window = createDefaultWindow();
    if (!m_embedder->runFlutter(m_window, m_assetsPath.c_str(), m_icudtl.c_str(), argc, (const char **)argv)) {
        return -1;
    }

    m_window->swapBuffer();
    // m_window->update();

    return 0;
}

}
}

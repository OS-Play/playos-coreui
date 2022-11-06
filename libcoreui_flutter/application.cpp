#include "coreui/flutter/application.h"
#include "coreui/application.h"

#include "coreui/window.h"

#include <sys/stat.h>
#include <string.h>


namespace playos {
namespace flutter {

static const char *assetsPathes[] = {
    "resources/flutter_assets",
    "../resources/flutter_assets",
};

static const char *icudtlPathes[] = {
    "resources/icu/icudtl.dat",
    "/usr/share/flutter/resources/icu/icudtl.dat",
};

const char *aotFile = "app.so";


Application::Application(int argc, char **argv):
        playos::Application(argc, argv), m_embedder(Embedder::create()),
            m_assetsPath(), m_icudtl()
{
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
    if (!initAssetsPath() || !initIcudtlPath()) {
        return false;
    }

    m_window = createDefaultWindow();

    int ret = m_embedder->runFlutter(m_window, m_assetsPath, m_icudtl,
            execPath() + '/' +aotFile, argc, (const char **)&argv[0]);
    if (!ret) {
        return -1;
    }

    return 0;
}

bool Application::initAssetsPath()
{
    if (m_assetsPath.empty()) {
        m_assetsPath = getFistPathExists(&assetsPathes[0], sizeof(assetsPathes)/sizeof(const char *), false);
        return !m_assetsPath.empty();
    }

    return true;
}

bool Application::initIcudtlPath()
{
    if (m_icudtl.empty()) {
        m_icudtl = getFistPathExists(&icudtlPathes[0], sizeof(icudtlPathes)/sizeof(const char *), true);
        return !m_icudtl.empty();
    }

    return true;
}

std::string Application::getFistPathExists(const char **pathes, size_t size, bool isFile)
{
    char path[256];
    auto _execPath = execPath(argv[0]);

    for (int i = 0; i < size; ++i) {
        path[0] = '\0';
        if (pathes[i][0] != '/') {
            strcpy(path, _execPath.c_str());
            if (path[_execPath.size() - 1] != '/') {
                path[_execPath.size()] = '/';
                path[_execPath.size()+1] = '\0';
            }
        }

        strcat(path, pathes[i]);

        struct stat st;
        if(stat(path, &st) == 0) {
            if(!isFile && S_ISDIR(st.st_mode)) {
                return std::string(path);
            } else if (isFile && S_ISREG(st.st_mode)) {
                return std::string(path);
            }
        }
    }

    return "";
}

void Application::parseArgs()
{
    playos::Application::parseArgs();

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--assets") == 0) {
            if (i+1 < argc && argv[i+1][0] != '-') {
                struct stat st;
                if(stat(argv[i+1], &st) == 0) {
                    if(S_ISDIR(st.st_mode)) {
                        m_assetsPath = std::string(argv[i+1]);
                    }
                }

                ++i;
            }
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--icudtl") == 0) {
            if (i+1 < argc && argv[i+1][0] != '-') {
                struct stat st;
                if(stat(argv[i+1], &st) == 0) {
                    if(S_ISREG(st.st_mode)) {
                        m_icudtl = std::string(argv[i+1]);
                    }
                }

                ++i;
            }
        }
    }
}

}
}

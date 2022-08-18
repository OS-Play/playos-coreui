#ifndef __COREUI_FLUTTER_APPLICATION_H__
#define __COREUI_FLUTTER_APPLICATION_H__

#include "coreui/application.h"
#include "../../../coreui_embedder.h"

#include <memory>
#include <string>

namespace playos {
namespace flutter {

class Application: public playos::Application {
public:
    Application(int argc, char **argv, const char *appPath, const char *icudtl);
    virtual ~Application();

    /**
     * @brief Create a Window object for flutter
     * 
     * @return std::shared_ptr<Window> 
     */
    virtual std::shared_ptr<Window> createDefaultWindow();

    /**
     * @brief Get the Window object of flutter
     * 
     * @return std::shared_ptr<Window> 
     */
    std::shared_ptr<Window> getWindow() {
        return m_window;
    }

protected:
    virtual int onInit();

private:
    std::shared_ptr<Window> m_window;
    std::unique_ptr<Embedder> m_embedder;

    std::string m_assetsPath;
    std::string m_icudtl;
};

}
}

#endif

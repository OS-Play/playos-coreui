#include <iostream>

#include "application.h"

int main(int argc, char *argv[])
{
    std::string assetsPath = playos::Application::execPath(argv[0]) + "/assets";
    const char *icudtl = "/prodata/flutter/engine/src/third_party/icu/common/icudtl.dat";
    playos::PanelApplication app(argc, argv, assetsPath.c_str(), icudtl);

    return app.run();
}

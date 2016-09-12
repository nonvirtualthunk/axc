#include<stdio.h>
#include <vector>
#include <iostream>
#include <testbeds/Testbed.h>


int main(int argc, char **argv) {
    printf("Hello there.\n");

    if (argc <= 1) {
        Application app;
        app.run();
    } else {
        std::string name(argv[1]);
        auto testbedOpt = Testbed::getTestbedsByName().get(name);
        if (testbedOpt.isPresent()) {
            GameApplication app;
            testbedOpt.get()->init(app, app.gameEngine, app.graphicsEngine, app.controlEngine);
            app.run();
        } else {
            Noto::error("Could not initialize testbed named {}", name);
            Noto::warn("Available testbeds are:");
            auto iter = Testbed::getTestbedsByName().begin();
            while (iter != Testbed::getTestbedsByName().end()) {
                Noto::warn("\t{}", iter->first);
                iter++;
            }
        }
    }

    return 0;
}
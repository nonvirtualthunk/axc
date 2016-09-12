//
// Created by Sam Bock on 8/17/16.
//

#include "WindowingSystemTestbed.h"


#include <gui/WindowingSystem.h>
#include <application/ResourceManager.h>
#include <core/ArxString.h>

WindowingSystemTestbed::WindowingSystemTestbed(const string &name) : Testbed(name) {
    Noto::info("Initializing windowing system testbed");
}

void WindowingSystemTestbed::init(GameApplication &app, GameEngine &gameEngine, GraphicsEngine &graphicsEngine,
                                  ControlEngine &controlEngine) {

    controlEngine.addComponent<WindowingSystem>();

    Widget * w = controlEngine.controlData(WindowingSystemData).topLevelWidget;
    w->backgroundImage = ResourceManager::image("ui/backgrounds/fancyBackground.png");
    std::string str;

    Widget * child1 = new Widget(w);
    child1->_x.value = 0;
    child1->_y.value = 0;
    child1->_width.value = 100;
    child1->_height.value = 100;
    child1->setBackground("greenMinorStyledBorder");


    Widget * child2 = new Widget(w);
    child2->_x.relativeTo = w;
    child2->_x.positionType = pPercentage;
    child2->_x.percentage = 0.5f;
    child2->_y.relativeTo = w;
    child2->_y.positionType = pPercentage;
    child2->_y.percentage = 0.5f;
    child2->_width.relativeTo = w;
    child2->_width.dimensionType = dPercentage;
    child2->_width.percentage = 0.50f;
    child2->_height.relativeTo = w;
    child2->_height.dimensionType = dPercentage;
    child2->_height.percentage = 0.25f;
    child2->setBackground("styledBorder_wood");
}

WindowingSystemTestbed * WindowingSystemTestbed::instance = new WindowingSystemTestbed("widget");

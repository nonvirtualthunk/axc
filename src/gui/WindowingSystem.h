//
// Created by Sam Bock on 8/17/16.
//

#ifndef TEST2_WINDOWINGSYSTEM_H
#define TEST2_WINDOWINGSYSTEM_H


#include <UnitOfMeasure.h>
#include <Event.h>
#include <engine/Engine.h>
#include "Widget.h"
#include <core/AuxData.h>

class EventBus;
class Shader;

class WindowingSystem : public ControlComponent {
protected:
    WidgetDrawContext drawContext;
    std::shared_ptr<Shader> shader;

public:
    WindowingSystem(ControlEngine &controlEngien);

    void addWidget(Widget * widget);

    void update(Time dt);
    void draw();

    Widget * getTopLevelWidget() ;
};

DECL_AUX_DATA(ControlAuxData, WindowingSystemData) {
public:
    Widget * topLevelWidget;

    WindowingSystemData_t();
};




#endif //TEST2_WINDOWINGSYSTEM_H

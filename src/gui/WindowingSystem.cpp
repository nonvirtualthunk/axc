//
// Created by Sam Bock on 8/17/16.
//

#include <GUIEvents.h>
#include <application/ResourceManager.h>
#include "WindowingSystem.h"
#include <graphics/Shader.h>
#include <application/Application.h>

void WindowingSystem::addWidget(Widget *widget) {
    auto topLevelWidget = getTopLevelWidget();
    widget->parent = topLevelWidget;
    topLevelWidget->children.add(widget);
}

void WindowingSystem::update(Time dt) {
    auto topLevelWidget = getTopLevelWidget();
    topLevelWidget->updateInternal();

    // if it's clean, mark it dirty
    drawContext.vbo->changeState(VBOs::Clean, VBOs::Dirty);

    // if it's dirty (explicitly, or because we just moved it from clean), start updating
    if (drawContext.vbo->changeState(VBOs::Dirty, VBOs::Updating)) {
        drawContext.vbo->clear();

        drawContext.revision++;
        topLevelWidget->draw(drawContext);

        posit(drawContext.vbo->changeState(VBOs::Updating, VBOs::Updated), "Could not move windowing vbo to updated");
    }
}

void WindowingSystem::draw() {
    auto topLevelWidget = getTopLevelWidget();

    auto proj = glm::ortho<float>(0, topLevelWidget->_width.value, topLevelWidget->_height.value,0,-100,100);
    auto mv = glm::mat4();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    shader->bind();
    shader->setUniform("ModelViewMatrix", mv);
    shader->setUniform("ProjectionMatrix", proj);
    shader->setUniform("MainTexture", 0);
    shader->setUniform("FontTexture", 1);
    drawContext.textureBlock->bind(0);
    drawContext.fontTextureBlock->bind(1);
    drawContext.vbo->drawElements(GL_DYNAMIC_DRAW);
}

WindowingSystem::WindowingSystem(ControlEngine *controlEngine) :
        ControlComponent(controlEngine),
        drawContext(1) {

    controlBus.onEvent<ResizeEvent>([&](ResizeEventPtr resize) {
        auto topLevelWidget = getTopLevelWidget();
        topLevelWidget->_width.value = (int) resize->framebufferDimensions.x;
        topLevelWidget->_height.value = (int) resize->framebufferDimensions.y;
    });

    shader = ResourceManager::shader("shaders/windowing/Windowing");

    auto topLevelWidget = getTopLevelWidget();
    topLevelWidget->_width.value = Application::inst->frameDimensions.x;
    topLevelWidget->_height.value = Application::inst->frameDimensions.y;
}

Widget *WindowingSystem::getTopLevelWidget()  {
    return aux(WindowingSystemData).topLevelWidget;
}


WindowingSystemData_t::WindowingSystemData_t() :
        topLevelWidget(new Widget(nullptr)) {
    topLevelWidget->_width.value = 1;
    topLevelWidget->_height.value = 1;
}

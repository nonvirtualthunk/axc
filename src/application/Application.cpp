//
// Created by nvt on 8/3/16.
//

#include "Application.h"
#include "Noto.h"
#include "GUIEvents.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <glm/ext.hpp>
#include <core/Predef.h>
#include <graphics/AxGL.h>


GLFWwindow* window;
GLFWmonitor* fullscreenMonitor;
Application* Application::inst = NULL;


Application::Application() {
    Application::inst = this;
}


void Application::run() {
    init();
    loop();

    shutdown();
}

void Application::shutdown() {

    glfwDestroyWindow(window);

    glfwTerminate();
}

void Application::loop() {
    glClearColor(0, 0, 0, 1);

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        AxGL::setViewport(0,0,frameDimensions.x,frameDimensions.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double curTime = glfwGetTime();
        double delta = curTime - lastTime;
        if (delta > (0.01666667 * 2.1)) {
            fprintf(stderr, "update: %f\n", delta);
        }
        lastTime = curTime;

        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
        lastSwap = Milliseconds(epochMillisSteady());
    }
}

void Application::init() {
    // Initialize glfw
    if (!glfwInit()) {
        Noto::error("Could not initialize glfw");
        exit(1);
    }

    // Configure our window
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, resizable); // the window will be resizable
    glfwWindowHint(GLFW_DECORATED, false);

    glfwSetErrorCallback([](int errCode, const char *msg) {
        Noto::error("GLFW error[{}]: {}", errCode, msg);
    });


    fullscreenMonitor = glfwGetPrimaryMonitor();
    GLFWmonitor * monitor = NULL;
    if (fullscreen) {
        auto vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        windowDimensions.x = vidMode->width;
        windowDimensions.y = vidMode->height;
        monitor = fullscreenMonitor;
    }
    window = glfwCreateWindow(windowDimensions.x, windowDimensions.y, windowTitle, monitor, NULL);
    if (!window) {
        Noto::error("Could not create glfw window");
        exit(1);
    }

    updateWindowDimensions();
    glfwGetWindowPos(window, &windowPosition.x, &windowPosition.y);

    glfwSetKeyCallback(window,Application::keyCallback);
    glfwSetWindowSizeCallback(window,Application::windowCallback);
    glfwSetMouseButtonCallback(window, Application::mouseButtonCallback);
    glfwSetCursorPosCallback(window, Application::mouseMoveCallback);
    glfwSetScrollCallback(window, Application::scrollCallback);
    glfwSetCharModsCallback(window, Application::charCallback);
    glfwSetWindowPosCallback(window, Application::windowPosCallback);

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    // Enable v-sync
    glfwSwapInterval(swapInterval); // Neither 0 nor 1 make any significant difference
    // Make the window visible
    glfwShowWindow(window);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
}

void Application::updateWindowDimensions() {
    glfwGetWindowSize(window,&windowDimensions.x,&windowDimensions.y);
    Noto::info("Current window size: {}", glm::to_string(windowDimensions));
    glfwGetFramebufferSize(window, &frameDimensions.x, &frameDimensions.y);
    Noto::info("Frame buffer size: {}", glm::to_string(frameDimensions));
}

Time Application::nextExpectedSwap() const {
    return lastSwap + Seconds(1/60.0f);
}

void Application::keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && (isBitSet(mods,GLFW_MOD_CONTROL) || isBitSet(mods,GLFW_MOD_SUPER))) {
        glfwSetWindowShouldClose(win,true);
    } else if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
        Application::inst->fullPause = ! Application::inst->fullPause;
    } else if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        auto inst = Application::inst;
        if (!inst->fullscreen) {
            auto vidMode = glfwGetVideoMode(fullscreenMonitor);
            inst->savedWindowedDimensions = inst->windowDimensions;
            inst->savedWindowedPosition = inst->windowPosition;
            inst->windowDimensions.x = vidMode->width;
            inst->windowDimensions.y = vidMode->height;
            auto monitor = fullscreenMonitor;

            glfwSetWindowMonitor(win,monitor,0,0,vidMode->width,vidMode->height,vidMode->refreshRate);
        } else {
            glfwSetWindowMonitor(win,nullptr,
                                 inst->savedWindowedPosition.x,inst->savedWindowedPosition.y,
                                 inst->savedWindowedDimensions.x,inst->savedWindowedDimensions.y,
                                 GLFW_DONT_CARE);
        }
        inst->fullscreen = !inst->fullscreen;
    }

    if (action == GLFW_PRESS) {
        KeyboardMirror::setKeyDown(key, true);
    } else if (action == GLFW_RELEASE) {
        KeyboardMirror::setKeyDown(key, false);
    }
    KeyboardMirror::setActiveModifiers(mods);

    auto evt = std::make_shared<KeyEvent>(key,action == GLFW_PRESS,KeyModifiers(mods));
    Application::inst->onEvent(evt);
}

void Application::windowCallback(GLFWwindow * win, int width, int height) {
    Application::inst->updateWindowDimensions();
}

void Application::mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
    auto evt = std::make_shared<MouseButtonEvent>(
            MouseMirror::getCurrentMousePosition(), action == GLFW_PRESS, KeyModifiers(mods));
    Application::inst->onEvent(evt);
}

void Application::mouseMoveCallback(GLFWwindow *win, double x, double y) {
    glm::vec2 pos(x,y);
    glm::vec2 old = MouseMirror::getCurrentMousePosition();
    MouseMirror::setMousePosition(pos);
    bool isDrag = MouseMirror::isLeftDown() || MouseMirror::isRightDown();
    auto evt = std::make_shared<MouseMoveEvent>(!isDrag,isDrag,pos, pos - old, KeyboardMirror::getActiveModifiers());
    Application::inst->onEvent(evt);
}

void Application::scrollCallback(GLFWwindow *win, double dx, double dy) {
    glm::vec2 delta(dx,dy);
    auto evt = std::make_shared<ScrollEvent>(delta, KeyboardMirror::getActiveModifiers());
    Application::inst->onEvent(evt);
}

void Application::charCallback(GLFWwindow *win, unsigned int codePoint, int mods) {
    auto evt = std::make_shared<CharEvent>(codePoint, KeyModifiers(mods));
    Application::inst->onEvent(evt);
}

void Application::windowPosCallback(GLFWwindow *win, int x, int y) {
    Noto::info("Window pos recorded: {}, {}", x, y);
    Application::inst->windowPosition = glm::ivec2(x,y);
}

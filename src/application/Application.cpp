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


GLFWwindow* window;
Application* Application::inst = NULL;


Application::Application() {
    Application::inst = this;
}


void Application::run() {
    init();
    loop();

    glfwDestroyWindow(window);

    glfwTerminate();
}

void Application::loop() {
    glClearColor(0, 0, 0, 1);
    glViewport(0, 0, 800, 600);

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
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

    glfwSetErrorCallback([](int errCode, const char *msg) {
        Noto::error("GLFW error[{}]: {}", errCode, msg);
    });


    GLFWmonitor * monitor = NULL;
    if (fullscreen) {
        auto vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        windowDimensions.x = vidMode->width;
        windowDimensions.y = vidMode->height;
        monitor = glfwGetPrimaryMonitor();
    }
    window = glfwCreateWindow(windowDimensions.x, windowDimensions.y, windowTitle, monitor, NULL);
    if (!window) {
        Noto::error("Could not create glfw window");
        exit(1);
    }

    updateWindowDimensions();

    glfwSetKeyCallback(window,Application::keyCallback);

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    // Enable v-sync
    glfwSwapInterval(swapInterval); // Neither 0 nor 1 make any significant difference
    // Make the window visible
    glfwShowWindow(window);
}

void Application::updateWindowDimensions() {
    glfwGetWindowSize(window,&windowDimensions.x,&windowDimensions.y);
    Noto::info("Current window size: {}", glm::to_string(windowDimensions));
    glfwGetFramebufferSize(window, &frameDimensions.x, &frameDimensions.y);
    Noto::info("Frame buffer size: {}", glm::to_string(frameDimensions));
}

void Application::keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && (isBitSet(mods,GLFW_MOD_CONTROL) || isBitSet(mods,GLFW_MOD_SUPER))) {
        glfwSetWindowShouldClose(win,true);
    } else if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
        Application::inst->fullPause = ! Application::inst->fullPause;
    }

    if (action == GLFW_PRESS) {
        KeyboardMirror::setKeyDown(key, true);
    } else if (action == GLFW_RELEASE) {
        KeyboardMirror::setKeyDown(key, false);
    }
    KeyboardMirror::setActiveModifiers(mods);

    auto evt = std::make_shared<KeyEvent>(key,action == GLFW_PRESS,KeyModifiers(mods));
    Application::inst->onEvent(evt);

    Noto::info("Key event called back: {}",key);
}

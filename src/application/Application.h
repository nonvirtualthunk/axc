//
// Created by nvt on 8/3/16.
//

#ifndef TEST2_APPLICATION_H
#define TEST2_APPLICATION_H


//#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <memory>
#include <functional>
#include <UnitOfMeasure.h>

class GLFWwindow;
class Event;

class Application {
public:
    const char* windowTitle = "Window";
    bool resizable = true;
    int swapInterval = 1;
    bool fullscreen = false;
    glm::ivec2 windowPosition = glm::ivec2(0,0);
    glm::ivec2 windowDimensions = glm::ivec2(1680,1050);
    glm::ivec2 savedWindowedDimensions = glm::ivec2(800,600);
    glm::ivec2 savedWindowedPosition = glm::ivec2(0,0);
    glm::ivec2 frameDimensions = glm::ivec2(800,600);
    Time lastSwap = Seconds(0);

    bool fullPause = false;

    virtual void init();

    virtual void loop();

    virtual void shutdown();

    void updateWindowDimensions();


    static void keyCallback(GLFWwindow* win,int key,int scancode,int action,int mods);
    static void windowCallback(GLFWwindow * win, int width, int height);
    static void mouseButtonCallback(GLFWwindow * win, int button, int action, int mods);
    static void mouseMoveCallback(GLFWwindow * win, double x, double y);
    static void scrollCallback(GLFWwindow * win, double dx, double dy);
    static void charCallback(GLFWwindow * win, unsigned int codePoint, int mods);
    static void windowPosCallback(GLFWwindow *win, int x, int y);


    virtual void onEvent(std::shared_ptr<Event> e) {}
    virtual void update(Time dt) {}
    virtual void draw() {}

    Time nextExpectedSwap() const;
public:
    Application();

    virtual void run();

    static Application* inst;
};

#endif //TEST2_APPLICATION_H

//
// Created by nvt on 8/3/16.
//

#ifndef TEST2_APPLICATION_H
#define TEST2_APPLICATION_H


//#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

class GLFWwindow;
class EventBus;

class Application {
protected:
    const char* windowTitle = "Window";
    bool resizable = true;
    int swapInterval = 1;
    bool fullscreen = false;
    glm::ivec2 windowDimensions = glm::ivec2(800,600);
    glm::ivec2 frameDimensions = glm::ivec2(800,600);

    bool fullPause = false;

    EventBus* eventBus;

    void init();
    void loop();

    void updateWindowDimensions();


    static void keyCallback(GLFWwindow* win,int key,int scancode,int action,int mods);
public:
    Application();

    void run();

    static Application* inst;
};

#endif //TEST2_APPLICATION_H

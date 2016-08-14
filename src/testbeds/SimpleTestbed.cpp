//
// Created by nvt on 8/13/16.
//

#include <Noto.h>
#include "SimpleTestbed.h"
#include <graphics/VBO.h>
#include <graphics/StandardAttributeProfiles.h>
#include <graphics/Shader.h>
#include <application/ResourceManager.h>

#include "core/Predef.h"


class SimpleGameComponent : public GameComponent {
public:
    SimpleGameComponent(GameEngine *gameEngine) : GameComponent(gameEngine) {}

    Time accumulated = Seconds(0);
    virtual void update(Time dt) override {
        accumulated = accumulated + dt;
        if (accumulated > Seconds(1)) {
            Noto::info("Second passed");
            accumulated = Seconds(0);
        }
    }
};

class SimpleGraphicsComponent : public GraphicsComponent {
public:
    VBO<SimpleAttributeProfile> vbo;
    std::shared_ptr<Shader> shader = ResourceManager::shader("shaders/simple/SimpleShader");

    glm::mat4x4 modelview;
    glm::mat4x4 projection;

    SimpleGraphicsComponent(GraphicsEngine *graphicsEngine) : GraphicsComponent(graphicsEngine) {}

    virtual void init() override {
        auto va = vbo.addQuad();

        if (vbo.changeState(VBOs::Dirty, VBOs::Updating)) {
            va[0].position = glm::vec3(-5.0f,-5.0f,0.0f);
            va[1].position = glm::vec3(5.0f,-5.0f,0.0f);
            va[2].position = glm::vec3(5.0f,5.0f,0.0f);
            va[3].position = glm::vec3(-5.0f,5.0f,0.0f);

            for (int i = 0; i < 4; ++i) {
                va[i].setColor(i/4.0f,0.25f,0.25f,1.0f);
                va[i].texCoord = glm::vec2(0.0f,0.0f);
            }

            vbo.changeState(VBOs::Updating,VBOs::Updated);
        } else {
            Noto::error("VBO was in wrong initial state");
        }
    }

    float theta = 0.0f;
    virtual void update(Time dt) override {
        theta = nextExpectedSwap().in(Seconds);
    }

    virtual void draw() override {
        float effTheta = theta;// + deltaSinceLastUpdate().in(Seconds);
        AxGL::checkError();
        modelview = glm::lookAt(
                glm::vec3(0.0f,0.0f,-10.0f),
                glm::vec3(0.0f,0.0f,0.0f),
                glm::vec3(0.0f,1.0f,0.0f)
        );
        modelview = glm::rotate(modelview,effTheta,glm::vec3(0.0f,0.0f,-1.0f));

        projection = glm::perspective(45.0f,(float)AxGL::activeViewport.w/(float)AxGL::activeViewport.h,0.5f,200.0f);

        glDisable(GL_DEPTH_TEST);

        shader->bind();
        AxGL::checkError();
        shader->setUniform("ModelViewMatrix", modelview);
        shader->setUniform("ProjectionMatrix", projection);
        AxGL::checkError();
        vbo.drawElements();
        AxGL::checkError();
    }
};


void SimpleTestbed::init(GameApplication& app, GameEngine &gameEngine, GraphicsEngine &graphicsEngine, ControlEngine &controlEngine) {
    app.fullscreen = true;
    gameEngine.addComponent<SimpleGameComponent>();

    graphicsEngine.addComponent<SimpleGraphicsComponent>();
}

SimpleTestbed::SimpleTestbed(const string &name) : Testbed(name) {}

SimpleTestbed * simpleTestbed = new SimpleTestbed("simple");


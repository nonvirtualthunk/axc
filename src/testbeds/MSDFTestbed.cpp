//
// Created by Sam Bock on 8/24/16.
//

#include <graphics/VBO.h>
#include <gui/Widget.h>
#include <gui/Font.h>
#include <application/ResourceManager.h>
#include <graphics/Shader.h>
#include <GUIEvents.h>
#include <GLFW/glfw3.h>
#include "MSDFTestbed.h"

float zoom = 24.0f;

MSDFTestbed::MSDFTestbed(const string &name) : Testbed(name) {
    Noto::info("Initializing windowing system testbed");
}

class MSDFGraphicsComponent : public GraphicsComponent {
public:
    VBO<WidgetAttributeProfile> vbo;
    FontPtr font;
    TextureBlock* tb = new TextureBlock(1024,1024);
    ShaderPtr shader;

    MSDFGraphicsComponent(GraphicsEngine *graphicsEngine) : GraphicsComponent(graphicsEngine) {}

    virtual void init() override {
        tb->minFilter = GL_NEAREST;
        tb->magFilter = GL_LINEAR;
        tb->mipmap = false;
        font = ResourceManager::font("fonts/Eurostile.ttf",tb);
        if (font->config.useMSDF) {
            shader = ResourceManager::shader("shaders/windowing/MSDF");
        } else {
            shader = ResourceManager::shader("shaders/windowing/Windowing");
        }
    }

    float lastZoom = zoom;
    virtual void update(Time dt) override {
        if (lastZoom != zoom) {
            lastZoom = zoom;
            vbo.changeState(VBOs::Clean, VBOs::Dirty);
        }

        float startY = Application::inst->frameDimensions.y / 2;

        if (vbo.changeState(VBOs::Dirty, VBOs::Updating)) {
            vbo.clear();
            Arx::String string = "The Quick Brown Fox Jumped Over The Lazy Dog";

            {
                WidgetAttributeProfile* q = vbo.addQuad();
                const auto& tc = tb->getOrElseUpdateCell(ResourceManager::image("black.png")).texCoords;
                for (int i = 0; i < 4; ++i) {
                    q[i].color = Color(255,0,0,255);
                    q[i].texCoord = tc[i];
                }
                q[0].position = glm::vec3(0.0f,startY-2,0.0f);
                q[1].position = glm::vec3(4000.0f,startY-2,0.0f);
                q[2].position = glm::vec3(4000.0f,startY,0.0f);
                q[3].position = glm::vec3(0.0f,startY,0.0f);
            }

            float x = 100.0f;
            float scale = zoom;
            for (int ci = 0; ci < string.size(); ++ci) {
                char c = string[ci];
                auto glyphData = font->glyphFor(c);
                const auto& texCoords = glyphData.textureCell.texCoords;

                WidgetAttributeProfile* q = vbo.addQuad();

                for (int i = 0; i < 4; ++i) {
                    q[i].color = Color(255,255,255,255);
                    q[i].texCoord = texCoords[i];
                }

                float percentHeight = glyphData.glyphBox.height;
                float percentWidth = glyphData.glyphBox.width;

                float w = max(scale * percentWidth, 1.0f);
                float h = max(scale * percentHeight, 1.0f);

                float descend = scale * glyphData.glyphBox.y;

                float lsb = glyphData.leftSideBearing * scale;
                float offset = 0.0f;
                q[0].position = glm::vec3((x+lsb)+offset,(startY+descend)+offset,0.0f);
                q[1].position = glm::vec3((x+lsb)+w+offset,(startY+descend)+offset,0.0f);
                q[2].position = glm::vec3((x+lsb)+w+offset,(startY+descend)+h+offset,0.0f);
                q[3].position = glm::vec3((x+lsb)+offset,(startY+descend)+h+offset,0.0f);

                x += glyphData.advanceWidth * scale;
            }

            vbo.changeState(VBOs::Updating,VBOs::Updated);
        }
    }

    virtual void draw() override {
        tb->bind();

        float ratio = float(Application::inst->frameDimensions.x) / float(Application::inst->frameDimensions.y);
        auto proj = glm::ortho<float>(0, Application::inst->frameDimensions.x,0, Application::inst->frameDimensions.y,-100,100);
        auto mv = glm::mat4();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        shader->bind();
        shader->setUniform("ModelViewMatrix", mv);
        shader->setUniform("ProjectionMatrix", proj);

        vbo.drawElements();
    }
};


class MSDFControlComponent : public ControlComponent {
public:
    MSDFControlComponent(ControlEngine *controlEngien) : ControlComponent(controlEngien) {}

    virtual void init() override {
        controlBus.onEvent<KeyEvent>([&](KeyEventPtr ke) {
           if (ke->key == GLFW_KEY_Z) {
               zoom += 1.0f;
           } else if (ke->key == GLFW_KEY_X) {
               zoom -= 1.0f;
           }
        });
    }

    virtual void update(Time dt) override {

    }
};

void MSDFTestbed::init(GameApplication &app, GameEngine &gameEngine, GraphicsEngine &graphicsEngine,
                                  ControlEngine &controlEngine) {

    graphicsEngine.addComponent<MSDFGraphicsComponent>();
    controlEngine.addComponent<MSDFControlComponent>();

}

MSDFTestbed * MSDFTestbed::instance = new MSDFTestbed("msdf");

//
// Created by Sam Bock on 9/24/16.
//

#include <testbeds/Testbed.h>
#include <axistential/components/LightComponent.h>
#include <axistential/data/WorldData.h>
#include <graphics/VBO.h>
#include <graphics/StandardAttributeProfiles.h>
#include <graphics/TextureBlock.h>
#include <application/ResourceManager.h>
#include <glm/ext.hpp>
#include <graphics/Shader.h>
#include <axistential/graphics/Common.h>
#include <math/Shapes3d.h>
#include <GUIEvents.h>
#include <GLFW/glfw3.h>


float pi = 3.14159f;
glm::vec3 eye(-20.0f,-20.0f,20.0f);
glm::vec2 angle(pi * 0.25f,-pi * 0.25f);

class SimpleTerrainGraphicsComponent : public GraphicsComponent {
public:
	SimpleTerrainGraphicsComponent(GraphicsEngine &graphicsEngine) : GraphicsComponent(graphicsEngine) {}

	VBO<SimpleAttributeProfile,uint32_t> vbo;
	TextureBlock * tb = new TextureBlock(1024,1024);
	std::shared_ptr<Shader> shader = ResourceManager::shader("shaders/simple/SimpleShader");


	virtual void update(Time dt) override {
		if (vbo.changeState(VBOs::Dirty, VBOs::Updating)) {
			vbo.clear();

			auto& terrain = world(Terrain);
			auto& light = world(Light);
			terrain.material.foreach([&](MaterialTalea* talea) {
				const auto& tc = tb->getOrElseUpdateCell(ResourceManager::image("blankBordered.png")).texCoords;

//				const auto& lightTal = light.globalLightValues.getTaleaRO(talea->position.x, talea->position.y, talea->position.z);
				FOR_XYZ_TALEA {
					if (Terrain_t::isSolid(talea->get(x,y,z))) {
						for (int q = 0; q < 6; ++q) {
							LightVoxel lv = light.globalLightValues(talea->position.x + x + cardinals[q].x,
																	talea->position.y + y + cardinals[q].y,
																	talea->position.z + z + cardinals[q].z);

							SimpleAttributeProfile * v = vbo.addQuad();

							for (int k = 0; k < 4; ++k) {
								v[k].position.x = talea->position.x + cubePoints[q][k].x + x;
								v[k].position.y = talea->position.y + cubePoints[q][k].y + y;
								v[k].position.z = talea->position.z + cubePoints[q][k].z + z;
								v[k].texCoord = tc[k];
								float lvf = pow(lv / float(MaxLight),2.2f);
//								if (x == 0 || y == 0) {
//									v[k].color = Color::fromPercent(lvf,0.0f,0.0f,1.0f);
//								} else {
									v[k].color = Color::fromPercent(lvf,lvf,lvf,1.0f);
//								}

							}
						}
					}
				};
			});


			vbo.changeState(VBOs::Updating, VBOs::Updated, true);
		}
	}

	virtual void draw() override {
		glm::vec3 forward(1.0f,0.0f,0.0f);
		glm::mat4 rot;
		rot = glm::rotate(rot, angle.x, glm::vec3(0.0f,0.0f,1.0f));
		rot = glm::rotate(rot, angle.y, glm::vec3(0.0f,1.0f,0.0f));
		glm::vec4 transformed = rot * glm::vec4(forward.x,forward.y,forward.z,1.0f);
		glm::vec4 up = rot * glm::vec4(0.0f,0.0f,1.0f,1.0f);

		glm::mat4 modelview = glm::lookAt(
				eye,
				eye + glm::vec3(transformed.x,transformed.y,transformed.z),
				glm::vec3(up.x,up.y,up.z)
		);
//		glm::mat4 modelview = glm::lookAt(
//				glm::vec3(0.0f,0.0f,-20.0f),
//				glm::vec3(0.0f,0.0f,10.0f),
//				glm::vec3(0.0f,1.0f,0.0f)
//		);
		glm::mat4 projection = glm::perspective(45.0f,AxGL::aspectRatio(),0.5f,200.0f);

		glEnable(GL_DEPTH_TEST);

		tb->bind();
		shader->bind();
		shader->setUniform("ModelViewMatrix", modelview);
		shader->setUniform("ProjectionMatrix", projection);
		shader->setUniform("tex0",0);
		vbo.drawElements();
	}
};

class CameraComponent : public ControlComponent {
public:
	CameraComponent(ControlEngine &controlEngine) : ControlComponent(controlEngine) {
		this->controlBus.onEvent<KeyEvent>([&](KeyEventPtr e) {
			if (e->modifiers.ctrl()) {
				if (e->key == GLFW_KEY_UP) {
					angle.y += 0.1f;
				} else if (e->key == GLFW_KEY_DOWN) {
					angle.y -= 0.1f;
				} else if (e->key == GLFW_KEY_LEFT) {
					angle.x -= 0.1f;
				} else if (e->key == GLFW_KEY_RIGHT) {
					angle.x += 0.1f;
				}
			} else {
				if (e->key == GLFW_KEY_UP) {
					eye.x += 1.0f;
				} else if (e->key == GLFW_KEY_DOWN) {
					eye.x -= 1.0f;
				} else if (e->key == GLFW_KEY_LEFT) {
					eye.y -= 1.0f;
				} else if (e->key == GLFW_KEY_RIGHT) {
					eye.y += 1.0f;
				} else if (e->key == GLFW_KEY_Z) {
					eye.z -= 1.0f;
				} else if (e->key == GLFW_KEY_X) {
					eye.z += 1.0f;
				}
			}
		});
	}

	virtual void update(Time dt) override {

	}
};

class LightingTestbed : public Testbed {
public:
	LightingTestbed(const string &name) : Testbed(name) {}

	virtual void init(GameApplication &app,
					  GameEngine &gameEngine,
					  GraphicsEngine &graphicsEngine,
					  ControlEngine &controlEngine) override {
		auto& terrain = gameEngine.world(Terrain);

		MatVoxel vox = 1 | Terrain_t::OpaqueMask | Terrain_t::SolidMask;
		std::function<void(int,int,int)> setVox = [&terrain, vox](int x,int y,int z){
			terrain.material.set(x,y,z, vox);
		};
		Cube<int>({-10,-10,-10}, {10,10,-10}).foreach(setVox);
		Cube<int>({-5,-5,0},{5,5,0}).foreach(setVox);
		Cube<int>({7,-5,-2},{7,5,10}).foreach(setVox);
		Cube<int>({7,-5,10},{15,5,10}).foreach(setVox);
		Cube<int>({7,-5,-2},{15,-5,10}).foreach(setVox);
		Cube<int>({7,5,-2},{15,5,10}).foreach(setVox);

		gameEngine.addComponent<LightComponent>();
		graphicsEngine.addComponent<SimpleTerrainGraphicsComponent>();
		controlEngine.addComponent<CameraComponent>();
	}

	static LightingTestbed * instance;
};

LightingTestbed * LightingTestbed::instance = new LightingTestbed("lighting");
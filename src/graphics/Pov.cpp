//
// Created by Sam Bock on 9/28/16.
//

#include <core/Predef.h>
#include "Pov.h"
#include "Shader.h"
#include "AxGL.h"
#include <GUIEvents.h>
#include <GLFW/glfw3.h>

void Pov::update() {
	if (lastUpdated == 0.0) {
		lastUpdated = elapsedSeconds();
	}
	double curTime = elapsedSeconds();
	double delta = curTime - lastUpdated;
	updateSelf(delta);
	lastUpdated = curTime;
}


void Pov::look() {
	const auto modelview = modelviewMatrix();
	const auto projection = projectionMatrix();

	if (AxGL::activeShaderObject.isPresent()) {
		Shader* shader = AxGL::activeShaderObject.get();
		if (shader->hasUniform("ModelViewMatrix")) {
			shader->setUniform("ModelViewMatrix", modelview);
		}
		if (shader->hasUniform("ProjectionMatrix")) {
			shader->setUniform("ProjectionMatrix", projection);
		}
		if (shader->hasUniform("ModelViewProjectionMatrix")) {
			shader->setUniform("ModelViewProjectionMatrix",projection * modelview);
		}
	} else {
		Noto::error("Cannot look() with Pov when no shader is bound");
	}
}

void EyeCamera::handleKeyEvent(std::shared_ptr<KeyEvent> evt) {
    if (evt->key == GLFW_KEY_LEFT) {

    }
}

void EyeCamera::updateSelf(double deltaSeconds) {

}

void EyeCamera::reset() {

}

void EyeCamera::setEye(Vec3f eye) {

}

void EyeCamera::setUp(Vec3f up) {

}

void EyeCamera::setForward(Vec3f forward) {

}

EyeCamera::EyeCamera(const Vec3f &eye, const Vec3f &up, const Vec3f &forward) : eye(eye), up(up), forward(forward) {}


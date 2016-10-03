//
// Created by Sam Bock on 9/28/16.
//

#ifndef TEST2_POV_H
#define TEST2_POV_H

#include <memory>
#include <glm/vec3.hpp>
#include <math/MathHelpers.h>
#include <glm/detail/type_mat.hpp>

class KeyEvent;

class Pov {
protected:
	double lastUpdated = 0.0;
public:
	virtual void look();

	virtual void handleKeyEvent(std::shared_ptr<KeyEvent> evt) = 0;
	virtual void updateSelf(double deltaSeconds) = 0;
	virtual void update() final;

	virtual void reset() = 0;
	virtual void setEye(Vec3f eye) = 0;
	virtual void setUp(Vec3f up) = 0;
	virtual void setForward(Vec3f forward) = 0;

	virtual glm::mat4 modelviewMatrix() = 0;
	virtual glm::mat4 projectionMatrix() = 0;
};


class EyeCamera : public Pov {
protected:
	Vec3f eye;
	Vec3f up;
	Vec3f forward;

public:
	EyeCamera(const Vec3f &eye, const Vec3f &up, const Vec3f &forward);

	virtual void handleKeyEvent(std::shared_ptr<KeyEvent> evt) override;

	virtual void updateSelf(double deltaSeconds) override;

	virtual void reset() override;

	virtual void setEye(Vec3f eye) override;

	virtual void setUp(Vec3f up) override;

	virtual void setForward(Vec3f forward) override;
};

#endif //TEST2_POV_H

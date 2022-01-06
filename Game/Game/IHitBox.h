#pragma once
#include <vector>
#include <Bullet3/btBulletDynamicsCommon.h>
#include "Model.h"

class Model;
class IHitBox
{
public:
	IHitBox();
	~IHitBox();

	virtual std::vector<btRigidBody*>& generateHitBoxes(Model* model) = 0;

	virtual void setRotationAroundCenter(const float angle) = 0;
	virtual void setWorldTransform(const glm::vec3& position, const glm::quat& rot) = 0;

protected:
	float centerRotation = 0.0f;
};


#pragma once
#include "IHitBox.h"

class TriangleHitbox :
	public IHitBox
{

public:
	~TriangleHitbox();

	std::vector<btRigidBody*>& generateHitBoxes(Model* model) override;

protected:
	void setRotationAroundCenter(const float angle) override;
	void setWorldTransform(const glm::vec3& position, const glm::quat& rot) override;

private:
	Model* model = nullptr;

	btCollisionShape* boxCollisionShape = nullptr;
	std::vector<btRigidBody*> rigidBodys;
};


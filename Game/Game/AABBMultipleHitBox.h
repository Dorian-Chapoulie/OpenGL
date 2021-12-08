#pragma once
#include "IHitBox.h"
#include <array>

class AABBMultipleHitBox :
	public IHitBox
{
	~AABBMultipleHitBox();
	std::vector<btRigidBody*>& generateHitBoxes(Model* model) override;

private:
	std::array<glm::vec3, 2> getBiggestHitBox() const;
	std::array<glm::vec3, 2> getMeshCenterAndSize(const std::vector<Vertex>& vertices) const;

protected:
	void setRotationAroundCenter(const float angle) override;
	void setWorldTransform(const glm::vec3& position, const glm::quat& rot) override;

private:
	Model* model = nullptr;

	std::vector<btCollisionShape*> boxCollisionShapes;
	std::vector<btRigidBody*> rigidBodys;
};


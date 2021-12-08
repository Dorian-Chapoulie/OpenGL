#pragma once
#include <glm/vec3.hpp>
#include <array>
#include "IHitBox.h"
#include "Model.h"
#include "Vertex.h"

class AABBHitbox :
	public IHitBox
{
public:
	~AABBHitbox();

	std::vector<btRigidBody*>& generateHitBoxes(Model* model) override;

private:
	std::array<glm::vec3, 2> getBiggestHitBox() const;
	std::array<glm::vec3, 2> getMeshCenterAndSize(const std::vector<Vertex>& vertices) const;

protected:
	void setRotationAroundCenter(const float angle) override;
	void setWorldTransform(const glm::vec3& position, const glm::quat& rot) override;

private:
	Model* model = nullptr;

	btCollisionShape* boxCollisionShape = nullptr;
	std::vector<btRigidBody*> rigidBodys;
};


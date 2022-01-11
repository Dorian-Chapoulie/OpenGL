#pragma once
#include "IHitBox.h"
class SkeletalHitbox :
    public IHitBox
{
public:
	~SkeletalHitbox();

	std::vector<btRigidBody*>& generateHitBoxes(Model* model) override;

private:
	std::array<glm::vec3, 2> getBiggestHitBox() const;
	std::array<glm::vec3, 2> getMeshCenterAndSize(const std::vector<Vertex>& vertices) const;

	void generateBonesHitboxes();
	void generateGlobalAABB();
protected:
	void setRotationAroundCenter(const float angle) override;
	void setWorldTransform(const glm::vec3& position, const glm::quat& rot) override;

private:
	Model* model = nullptr;

	btCollisionShape* boxCollisionShape = nullptr;
	std::vector<btRigidBody*> rigidBodys;
};


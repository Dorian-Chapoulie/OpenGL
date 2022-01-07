#pragma once
#include "HitBoxFactory.h"
#include "HitBoxFactory.h"
#include "Model.h"
class StaticModel :
	public Model
{
public:
	StaticModel(const std::string& path,
		const glm::vec3& position,
		HitBoxFactory::TYPE type = HitBoxFactory::TYPE::NONE,
		glm::vec3 scale = glm::vec3(1.0f),
		bool isAnimated = false
	);

	void setPosition(const glm::vec3& position) override;
	void setRotation(const glm::vec3& rotationAxis, float angle) override;
	std::vector<btRigidBody*>& getRigidBodys() override;

	IHitBox* getHitBox() override;

private:
	void loadModel(const std::string& path) override;
	IHitBox* hitbox;
};



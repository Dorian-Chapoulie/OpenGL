#pragma once
#include "HitBoxFactory.h"
#include "Model.h"
class DynamicModel :
	public Model
{
public:
	DynamicModel(const std::string& path,
		const glm::vec3& position,
		float weight,
		HitBoxFactory::TYPE type = HitBoxFactory::TYPE::NONE,
		glm::vec3 scale = glm::vec3(1.0f),
		bool isAnimated = false
	);

	void setPosition(const glm::vec3& position) override;
	std::vector<btRigidBody*>& getRigidBodys();
	IHitBox* getHitBox() override;

private:
	void loadModel(const std::string& path) override;

	IHitBox* hitbox;
	std::vector<btRigidBody*> rigidBodys;
};


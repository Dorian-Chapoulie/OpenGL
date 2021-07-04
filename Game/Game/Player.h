#pragma once
#include "Model.h"
#include <reactphysics3d/reactphysics3d.h>

class Player
{
public:
	Player(const std::string& bodyModelPath, const glm::vec3& position);
	~Player();

	glm::vec3 getPosition() const;
	
	virtual void setPosition(const glm::vec3& position);
	void setOrientation(const glm::vec3& orientation);

	virtual void draw(Shader& shader);
	void increaseCameraYOffset();
	void decreaseCameraYOffset();

	reactphysics3d::RigidBody* getRigidBody() const;
	Model* getModel() const;

private:
	glm::vec3 position = glm::vec3(1.0f);
	glm::vec3 orientation = glm::vec3(1.0f);

protected:
	Model* model = nullptr;
	float offsetCameraY = 25.0f;
	const float offsetCameraYStep = 1.0f;
	const float WEIGHT = 75.0f;
};


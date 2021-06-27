#pragma once
#include "Model.h"

class Player
{
public:
	Player(const std::string& bodyModelPath, const std::string& weaponModelPath, const glm::vec3& position);
	~Player();

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& position);
	void setOrientation(const glm::vec3& orientation);

	virtual void draw(Shader& shader);

	Model* getWeapon();

protected:
	glm::vec3 position = glm::vec3(1.f);
	glm::vec3 orientation = glm::vec3(1.f);
	Model* body = nullptr;
	Model* weapon = nullptr;
};


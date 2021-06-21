#pragma once
#include "Model.h"

class Player
{
public:
	Player(const std::string& bodyModelPath, const std::string& weaponModelPath, const glm::vec3& position);
	~Player();

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& position);
	virtual void draw(Shader& shader);

protected:
	glm::vec3 position = glm::vec3(1.f);
	Model* body = nullptr;
	Model* weapon = nullptr;
};


#pragma once
#include "Camera.h"
#include "Player.h"
class LocalPlayer :
    public Player
{
public:
	LocalPlayer(const std::string& bodyModelPath, const std::string& weaponModelPath, const glm::vec3& position);
	
	std::unique_ptr<Camera>& getCamera();
	void draw(Shader& shader) override;
private:
	std::unique_ptr<Camera>& camera = Camera::getInstance();
	
	glm::mat4 weaponModelMatrix = glm::mat4(0.1f);
	const float weaponOffsetX = 0.01f;
	const float weaponOffsetY = -0.024f;
	const float weaponOffsetZ = 0.227f;
};


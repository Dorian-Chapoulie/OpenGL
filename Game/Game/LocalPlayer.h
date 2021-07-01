#pragma once
#include "Camera.h"
#include "Player.h"
class LocalPlayer :
    public Player
{
public:
	LocalPlayer(const std::string& bodyModelPath, const glm::vec3& position);
	
	std::unique_ptr<Camera>& getCamera();

	void draw(Shader& shader) override;
	void setPosition(const glm::vec3& position) override;

private:
	std::unique_ptr<Camera>& camera = Camera::getInstance();
};


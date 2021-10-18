#pragma once
#include "Camera.h"
#include "Player.h"
class LocalPlayer :
    public Player
{
public:
	LocalPlayer(const std::string& bodyModelPath, const glm::vec3& position);
	
	std::unique_ptr<Camera>& getCamera();

	void setPosition(const glm::vec3& position) override;
	void setCameraPosition(const glm::vec3& position, const glm::vec3& size);
	void move(const glm::vec3& force);

private:
	std::unique_ptr<Camera>& camera = Camera::getInstance();
};


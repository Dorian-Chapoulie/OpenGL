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
	void move(bool forward, bool backward, bool left, bool right, bool jump, float delta);

private:
	std::unique_ptr<Camera>& camera = Camera::getInstance();
	const float SPEED = 50.0f;
	const float JUMP_SECONDS = 0.5f;
	float JUMP_FORCE = 2.0f;
	const float FALL_FORCE = -1.0f;
	bool isJumping = false;
};


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
	void setCameraPosition(const glm::vec3& position);
	void move(bool forward, bool backward, bool left, bool right, bool jump, float delta);

	bool canJump = true;
	bool isJumping = false;
	float lastJumpTimeStamp = 0;
	const float JUMP_TIME = 0.5; //seconds

private:
	std::unique_ptr<Camera>& camera = Camera::getInstance();
	const float SPEED = 1.0f; // 3.5f;
	const float JUMP_FORCE = 100.0f;

};


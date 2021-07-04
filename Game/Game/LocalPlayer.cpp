#include "LocalPlayer.h"
#include <reactphysics3d/reactphysics3d.h>

LocalPlayer::LocalPlayer(const std::string& bodyModelPath, const glm::vec3& position)
: Player(bodyModelPath, position)
{
	setPosition(position);
}

std::unique_ptr<Camera>& LocalPlayer::getCamera()
{
	return camera;
}

void LocalPlayer::setPosition(const glm::vec3& position)
{
	camera->setPosition(glm::vec3(position.x * 2, position.y + offsetCameraY, position.z * 2));
	Player::setPosition(position);
	//Player::setOrientation(camera->getFrontVector());
}

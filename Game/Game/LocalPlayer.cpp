#include "LocalPlayer.h"

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
	//Player::setPosition(position);
}

//TODO: remove size
void LocalPlayer::setCameraPosition(const glm::vec3& position, const glm::vec3& size) {
	camera->setPosition(
		glm::vec3(
			position.x,
			position.y + offsetCameraY,
			position.z
		)
	);
}

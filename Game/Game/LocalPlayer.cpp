#include "LocalPlayer.h"
#include <reactphysics3d/reactphysics3d.h>

LocalPlayer::LocalPlayer(const std::string& bodyModelPath, const glm::vec3& position)
: Player(bodyModelPath, position)
{
	camera->setPosition(position);
	Player::setPosition(position);
}

std::unique_ptr<Camera>& LocalPlayer::getCamera()
{
	return camera;
}

void LocalPlayer::draw(Shader& shader)
{
	/*const glm::mat4 mtx_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	const auto cameraPosition = glm::inverse(camera->getViewMatrix()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->position = glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	shader.setMatrix("view", mtx_trans);*/
	
	
	Player::draw(shader);
}

void LocalPlayer::setPosition(const glm::vec3& position)
{
	/*const glm::mat4 mtx_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	const auto cameraPosition = glm::inverse(camera->getViewMatrix()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	const glm::vec3 positionTmp = glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);*/
	
	Player::setPosition(position);
	camera->setPosition(glm::vec3(position.x, position.y + offsetCameraY, position.z));
}

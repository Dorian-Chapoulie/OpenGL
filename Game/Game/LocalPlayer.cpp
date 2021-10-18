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

void LocalPlayer::move(const glm::vec3& force)
{
	for (auto rigidBody : getModel()->getRigidBodys()) {

		std::cout << camera->getFrontVector().x << " " << camera->getFrontVector().z << std::endl;

		rigidBody->setLinearVelocity(btVector3(
			(camera->getFrontVector().x * force.z) / 10.0f,
			-1.0f,
			(camera->getFrontVector().z * force.z) / 10.0f
		));

		/*rigidBody->applyCentralForce(btVector3(
			force.x * camera->getFrontVector().x,
			force.y,
			force.z * camera->getFrontVector().z
		));*/
	}
}

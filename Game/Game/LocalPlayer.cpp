#include "LocalPlayer.h"
#include <thread>
#include <chrono>
#include <Bullet3/BulletDynamics/Dynamics/btRigidBody.h>

LocalPlayer::LocalPlayer(const std::string& bodyModelPath, const glm::vec3& position)
	: Player(bodyModelPath, position)
{

}

std::unique_ptr<Camera>& LocalPlayer::getCamera()
{
	return camera;
}

const glm::vec3& LocalPlayer::get_position()
{
	//return dynamicModel->get_hit_boxes()[0]->get_position();
	return dynamicModel->get_position();
}

void LocalPlayer::setPosition(const glm::vec3& position)
{
	//Player::setPosition(position);
}

void LocalPlayer::setCameraPosition(const glm::vec3& position) {

	const glm::vec3 pos = camera->getPosition() + position;

	camera->setPosition(
		glm::vec3(
			position.x,
			position.y + offsetCameraY,
			position.z
		)
	);
}

void LocalPlayer::move(bool forward, bool backward, bool left, bool right, bool jump, float delta)
{
	//if (!forward && !backward && !right && !left && !isJumping && !jump) return;

	glm::vec3 UP = camera->getUpVector();
	glm::vec3 view;
	view.x = cos(glm::radians(camera->getYaw())) * cos(glm::radians(0.0f));
	view.z = sin(glm::radians(camera->getYaw())) * cos(glm::radians(0.0f));
	glm::vec3 moveVector = view;

	if (!forward) {
		moveVector = glm::vec3(0.0f);
	}
	if (backward) {
		moveVector = -view;
	}
	if (left) {
		moveVector += glm::cross(-view, UP);
	}
	if (right) {
		moveVector += glm::cross(view, UP);
	}

	if (jump && !isJumping) {
		isJumping = true;
		std::thread([&]() {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(0.15s);
			isJumping = false;
			}).detach();
	}

	const float compensation = (1.0f + delta);
	moveVector *= SPEED * compensation;

	HitBox* body = dynamicModel->get_hit_boxes()[0];
	btRigidBody* rigidBody = body->get_rigid_body();
	rigidBody->activate();
	rigidBody->setLinearVelocity(btVector3(
		moveVector.x,
		isJumping ? JUMP_FORCE : FALL_FORCE,
		moveVector.z
	));
}

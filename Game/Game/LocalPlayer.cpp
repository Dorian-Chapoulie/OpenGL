#include "LocalPlayer.h"

LocalPlayer::LocalPlayer(const std::string& bodyModelPath, const std::string& weaponModelPath, const glm::vec3& position)
: Player(bodyModelPath, weaponModelPath, position)
{
	camera->setPosition(position);
	weaponModel[3][0] = weaponOffsetX;
	weaponModel[3][1] = weaponOffsetY;
	weaponModel[3][2] = weaponOffsetZ;
}

std::unique_ptr<Camera>& LocalPlayer::getCamera()
{
	return camera;
}

void LocalPlayer::draw(Shader& shader)
{
	glm::mat4 mtx_trans = glm::mat4(1.0f);
	mtx_trans = glm::translate(mtx_trans, glm::vec3(0.0f, 0.0f, -3.0f));
	shader.setMatrix("view", mtx_trans);
	shader.setMatrix("model", weaponModel);


	const auto cam_world = glm::inverse(camera->getViewMatrix()) * glm::vec4(0, 0, 0, 1);
	this->position = glm::vec3(cam_world.x, cam_world.y, cam_world.y);
	shader.setVec3("pos", position);

	Player::draw(shader);

	shader.setVec3("pos", glm::vec3(1.0f));
	shader.setMatrix("model", glm::mat4(1.0f));

	
}

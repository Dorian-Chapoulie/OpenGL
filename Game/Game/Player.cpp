#include "Player.h"

#include "DynamicModel.h"

Player::Player(const std::string& bodyModelPath, const glm::vec3& position)
{
	this->position = position;
	initModel(bodyModelPath);
}

Player::~Player()
{
	delete model;
}

//todo: remove this
glm::vec3 Player::getPosition() const
{
	return model->getPosition();
}

void Player::setPosition(const glm::vec3& position)
{
	this->position = position;
	//this->model->setPosition(this->position);
	//this->model->setWorldTransform(this->position, glm::quat(glm::vec3(0, 0, 0)));
}

void Player::setOrientation(const glm::vec3& orientation)
{
	this->orientation = orientation;
}

void Player::changeModel(const std::string& modelPath)
{
	if (this->modelPath == modelPath) return;
	initModel(modelPath);
}
//TODO: replace this.draw by getModel().draw()
void Player::draw(Shader& shader)
{
	model->draw(shader);
}

void Player::increaseCameraYOffset()
{
	this->offsetCameraY += offsetCameraYStep;
}

void Player::decreaseCameraYOffset()
{
	this->offsetCameraY -= offsetCameraYStep;
}

DynamicModel* Player::getModel() const
{
	return model;
}

void Player::initModel(const std::string& modelPath)
{
	delete model;
	this->modelPath = modelPath;
	model = new DynamicModel(modelPath, position, 2.0f, HitBoxFactory::AABB, glm::vec3(3.0f));
}

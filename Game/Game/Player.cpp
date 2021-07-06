#include "Player.h"

Player::Player(const std::string& bodyModelPath, const glm::vec3& position)
{
	this->position = position;
	initModel(bodyModelPath);
}

Player::~Player()
{
	delete model;
}

glm::vec3 Player::getPosition() const
{
	return position;
}

void Player::setPosition(const glm::vec3& position)
{
	this->position = position;
	this->model->setPosition(this->position);
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

Model* Player::getModel() const
{
	return model;
}

void Player::initModel(const std::string& modelPath)
{
	delete model;
	this->modelPath = modelPath;
	model = new Model(modelPath, MODEL_TYPE::RIGID_BODY, position, false, true);
}

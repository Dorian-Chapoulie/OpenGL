#include "Player.h"

Player::Player(const std::string& bodyModelPath, const std::string& weaponModelPath, const glm::vec3& position)
{
	//this->body = new Model(bodyModelPath);
	this->weapon = new Model(weaponModelPath);
	this->position = position;
}

Player::~Player()
{
	delete body;
	delete weapon;
}

glm::vec3 Player::getPosition() const
{
	return position;
}

void Player::setPosition(const glm::vec3& position)
{
	this->position = position;
}

void Player::draw(Shader& shader)
{
	//body->draw(shader);
	weapon->draw(shader);
}

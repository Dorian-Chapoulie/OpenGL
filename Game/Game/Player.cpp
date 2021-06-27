#include "Player.h"

Player::Player(const std::string& bodyModelPath, const std::string& weaponModelPath, const glm::vec3& position)
{
	//this->body = new Model(bodyModelPath);
	this->weapon = new Model(weaponModelPath, false);
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

void Player::setOrientation(const glm::vec3& orientation)
{
	this->orientation = orientation;
}

void Player::draw(Shader& shader)
{
	//body->draw(shader);
	weapon->draw(shader);
}

Model* Player::getWeapon()
{
	return weapon;
}

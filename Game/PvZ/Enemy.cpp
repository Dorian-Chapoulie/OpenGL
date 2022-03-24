#include "Enemy.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "EZNgine.h"
#include "StaticModel.h"
#include "Player.h"


Enemy::Enemy(const glm::vec3 position)
{
	weight = 0.0f;
	setModel(new StaticModel(filePath, position, HitBoxFactory::AABB, glm::vec3(scale)));
}

void Enemy::onInit(btDiscreteDynamicsWorld* world)
{
	Entity::onInit(world);
}

void Enemy::onCollide(Entity* other)
{
	const PlayerEntity* playerEntity = dynamic_cast<PlayerEntity*>(other);
	const BasicBullet* bullet = dynamic_cast<BasicBullet*>(other);
	if (playerEntity)
	{
		//std::cout << "Enemy collide with player: " << std::endl;
	}
	else if (bullet)
	{
		//std::cout << "Enemy collide with bullet" << std::endl;
	}
}

void Enemy::onUpdate(double timeStamp, btDiscreteDynamicsWorld* world)
{
	timeBuffer += timeStamp;

	const glm::vec3 localPlayerPosition = EZNgine::localPlayer->getPosition();
	const glm::mat4 transformation = glm::inverse(glm::lookAt(model->getPosition(), localPlayerPosition, { 0, 1, 0 }));
	glm::vec3 s;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transformation, s, rotation, translation, skew, perspective);
	model->setMatrix(transformation * glm::scale(model->getScale()));

	if (timeBuffer >= fireRate)
	{
		const glm::vec3 dir = localPlayerPosition - model->getPosition();
		shoot(dir);
		bullets.back()->onInit(world);
		timeBuffer = 0;
	}
	Entity::onUpdate(timeStamp, world);
}

void Enemy::shoot(glm::vec3 direction)
{
	glm::vec3 pos = model->getPosition();
	pos.y = 20.0f;
	bullets.emplace_back(new BasicBullet(pos, direction));
}

void Enemy::removeBullet(int index)
{
	bullets.erase(bullets.begin() + index);
}

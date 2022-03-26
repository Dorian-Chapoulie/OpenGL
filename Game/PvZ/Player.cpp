#include "Player.h"

#include <glm/gtx/matrix_decompose.hpp>

#include "Enemy.h"
#include "EZNgine.h"

PlayerEntity::PlayerEntity(LocalPlayer* localPlayer)
{
	setModel(localPlayer->getModel());
}

void PlayerEntity::onInit(btDiscreteDynamicsWorld* world)
{
	Entity::onInit(world);
}

void PlayerEntity::onCollide(Entity* other)
{
	BasicBullet* bullet = dynamic_cast<BasicBullet*>(other);
	const Enemy* enemy = dynamic_cast<Enemy*>(other);
	if (bullet)
	{
		bullet->state = ENTITY_STATE::DEAD;
		std::cout << "Player collide with bullet" << std::endl;
	}
	else if (enemy)
	{
		std::cout << "Player collide with enemy" << std::endl;
	}
}

void PlayerEntity::onUpdate(double timeStamp, btDiscreteDynamicsWorld* world)
{
	timeBuffer += timeStamp;
	Entity::onUpdate(timeStamp, world);
}

void PlayerEntity::shoot(btDiscreteDynamicsWorld* world)
{
	if (timeBuffer < fireRate) return;

	const glm::vec3 cameraPos = EZNgine::localPlayer->getCamera()->getPosition();

	constexpr  float power = -10.f;
	const glm::vec3 forward = glm::mat3(glm::inverse(EZNgine::localPlayer->getCamera()->getViewMatrix())) * glm::vec3(0, 0, 1);
	glm::vec3 origin = cameraPos + forward;
	const glm::vec3 vel = forward * power;

	origin.y += 5.0f;
	bullets.emplace_back(new BasicBullet(origin, vel));
	bullets.back()->onInit(world);
	//timeBuffer = 0;
}

void PlayerEntity::removeBullet(int index)
{
	bullets.erase(bullets.begin() + index);
}
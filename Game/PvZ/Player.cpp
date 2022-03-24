#include "Player.h"

#include "Enemy.h"

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
	Entity::onUpdate(timeStamp, world);
}

#include "Trigger.h"

#include "Player.h"
#include "StaticModel.h"

Trigger::Trigger()
{
}

Trigger::Trigger(std::function<void(void*)> foo, glm::vec3 position, float scale)
	: onCollideWithPlayerCallback(std::move(foo))
{
	weight = 0.0f;
	setModel(new StaticModel(filePath, position, HitBoxFactory::AABB, glm::vec3(scale)));
}

void Trigger::onInit(btDiscreteDynamicsWorld* world)
{
	Entity::onInit(world);
}

void Trigger::onCollide(Entity* other)
{
	const PlayerEntity* player = dynamic_cast<PlayerEntity*>(other);
	if (player)
	{
		onCollideWithPlayerCallback((void*)player);
	}
}

void Trigger::onUpdate(double timeStamp, btDiscreteDynamicsWorld* world)
{
	Entity::onUpdate(timeStamp, world);
}

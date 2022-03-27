#include "BasicBullet.h"
#include "DynamicModel.h"
#include "Enemy.h"
#include "Player.h"

BasicBullet::BasicBullet(glm::vec3 position, glm::vec3 direction)
	: direction(direction)
{
	weight = 1.0f;
	MAX_LIFETIME = 5.0f;
	setModel(new DynamicModel(filePath, position, weight, HitBoxFactory::AABB));
}

void BasicBullet::onInit(btDiscreteDynamicsWorld* world)
{
	for (auto* rigidBody : model->getRigidBodys()) {
		world->addRigidBody(rigidBody);
		rigidBody->setGravity(customGravity);
		rigidBody->applyCentralForce(btVector3(direction.x, direction.y, direction.z));
	}
}

void BasicBullet::onCollide(Entity* other)
{
	PlayerEntity* playerEntity = dynamic_cast<PlayerEntity*>(other);
	const Enemy* enemy = dynamic_cast<Enemy*>(other);
	if (playerEntity)
	{
		playerEntity->applyDamage(this->damage);
	}
	else if (enemy)
	{
		std::cout << "Bullet collide with enemy" << std::endl;
	}
	state = ENTITY_STATE::DEAD;
}

void BasicBullet::onUpdate(double timeStamp, btDiscreteDynamicsWorld* world)
{
	Entity::onUpdate(timeStamp, world);
}

float BasicBullet::getDamage() const
{
	return damage;
}

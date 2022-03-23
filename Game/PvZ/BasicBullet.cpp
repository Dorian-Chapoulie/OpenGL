#include "BasicBullet.h"
#include "DynamicModel.h"

BasicBullet::BasicBullet(glm::vec3 position, glm::vec3 direction)
	: direction(direction)
{
	weight = 1.0f;
	model = new DynamicModel(filePath, position, weight, HitBoxFactory::AABB);
}

void BasicBullet::onInit(btDiscreteDynamicsWorld* world)
{
	for (auto* rigidBody : model->getRigidBodys()) {
		world->addRigidBody(rigidBody);
		rigidBody->setGravity(customGravity);
		rigidBody->applyCentralForce(btVector3(direction.x, 0.0f, direction.z));
	}
}

void BasicBullet::onCollide(Entity* other)
{
	std::cout << "Bullet collide" << std::endl;
}

void BasicBullet::onUpdate(double timeStamp, btDiscreteDynamicsWorld* world)
{
	Entity::onUpdate(timeStamp, world);
}

#include "Entity.h"

void Entity::onInit(btDiscreteDynamicsWorld* world)
{
	for (auto* rigidBody : model->getRigidBodys()) {
		world->addRigidBody(rigidBody);
		rigidBody->setUserPointer(this);
	}
}

void Entity::onCollide(Entity* other)
{
}

void Entity::onUpdate(double timeStamp, btDiscreteDynamicsWorld* world)
{
}

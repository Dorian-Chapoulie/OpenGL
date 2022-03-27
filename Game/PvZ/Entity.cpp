#include "Entity.h"

Entity::~Entity()
{
	delete model;
}

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
	lifeTime += timeStamp;
}

void Entity::setModel(Model* model)
{
	this->model = model;
	for (btRigidBody* r : model->getRigidBodys())
	{
		r->setUserPointer(this);
	}
}

float Entity::getMaxLifeTime() const
{
	return MAX_LIFETIME;
}

float Entity::getLifeTime() const
{
	return lifeTime;
}

float Entity::getLife() const
{
	return life;
}

void Entity::applyDamage(float amount)
{
	this->life -= amount;
}

void Entity::setLife(float value)
{
	this->life = value;
}

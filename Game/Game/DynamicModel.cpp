#include "DynamicModel.h"

#include "ModelLoader.h"
#include "HitBoxFactory.h"

DynamicModel::DynamicModel(
	const std::string& path,
	const glm::vec3& position,
	float weight,
	HitBoxFactory::TYPE type,
	glm::vec3 scale)
	: Model(path, position, weight, scale)
{
	loadModel(path);
	hitbox = HitBoxFactory::getInstance()->getHitBoxes(type);
	rigidBodys = hitbox->generateHitBoxes(this);
}

void DynamicModel::setPosition(const glm::vec3& position)
{
	this->position = position;
	this->modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale);
	btTransform transform;
	transform.setIdentity();
	for (auto* rigidBody : rigidBodys) {
		transform.setOrigin(btVector3(position.x, position.y, position.z));
		rigidBody->setWorldTransform(transform);
		rigidBody->getMotionState()->setWorldTransform(transform);
	}
}

std::vector<btRigidBody*>& DynamicModel::getRigidBodys()
{
	return rigidBodys;
}

IHitBox* DynamicModel::getHitBox()
{
	return hitbox;
}

void DynamicModel::loadModel(const std::string& path)
{
	modelData = ModelLoader::getInstance()->loadModel(path, ModelLoader::DEFAULT);
}

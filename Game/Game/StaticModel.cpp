#include "StaticModel.h"

#include "HitBoxFactory.h"
#include "ModelLoader.h"

StaticModel::StaticModel(
	const std::string& path,
	const glm::vec3& position,
	HitBoxFactory::TYPE type,
	glm::vec3 scale,
	bool isAnimated)
	: Model(path, position, 0.0f, scale, isAnimated)
{
	loadModel(path);
	hitbox = HitBoxFactory::getInstance()->getHitBoxes(type);
	rigidBodys = hitbox->generateHitBoxes(this);
}

void StaticModel::setPosition(const glm::vec3& position)
{
	Model::setPosition(position);
	btTransform transform;
	transform.setIdentity();
	for (auto* rigidBody : rigidBodys) {
		if (rigidBody->getMotionState() != nullptr) {
			transform.setOrigin(btVector3(position.x, position.y, position.z));
			rigidBody->setWorldTransform(transform);
			rigidBody->getMotionState()->setWorldTransform(transform);
		}
	}
}

std::vector<btRigidBody*>& StaticModel::getRigidBodys()
{
	return rigidBodys;
}

IHitBox* StaticModel::getHitBox()
{
	return hitbox;
}

void StaticModel::loadModel(const std::string& path)
{
	if (isAnimated)
	{
		this->modelData = ModelLoader::getInstance()->loadModel(path, ModelLoader::SKELETAL);
		this->animation = new Animation(path, reinterpret_cast<SkeletalModelData&>(*modelData), modelMatrix);
	}
	else {
		this->modelData = ModelLoader::getInstance()->loadModel(path, ModelLoader::DEFAULT);
	}
}

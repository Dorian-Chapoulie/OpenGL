#include "SkeletalHitbox.h"
#include <glad/glad.h>
#include <array>
#include "MyMotionState.h"
#include "SkeletalLoader.h"

//TODO: FOR ALL HITBOXES (AABB AND MULTIPLE): refactor as compound for non animated models
SkeletalHitbox::~SkeletalHitbox()
{
	delete boxCollisionShape;
	rigidBodys.clear();
}

std::vector<btRigidBody*>& SkeletalHitbox::generateHitBoxes(Model* model)
{
	this->model = model;

	generateGlobalAABB();
	generateBonesHitboxes();

	return rigidBodys;
}

//TODO: replace by bullet getAABB()
std::array<glm::vec3, 2> SkeletalHitbox::getBiggestHitBox() const
{
	std::vector<Mesh*> meshes = model->getModelData()->meshes;
	std::array<glm::vec3, 2> sizeCenter = getMeshCenterAndSize(meshes[0]->getVertices());
	glm::vec3 maxSize = glm::vec3(sizeCenter[0]);
	for (Mesh* m : meshes)
	{
		const std::array<glm::vec3, 2> temp = getMeshCenterAndSize(m->getVertices());
		const glm::vec3 size = temp[0];
		if (size.x > maxSize.x && size.y > maxSize.y && size.z > maxSize.z)
		{
			maxSize = size;
			sizeCenter = temp;
		}
	}
	return sizeCenter;
}

std::array<glm::vec3, 2> SkeletalHitbox::getMeshCenterAndSize(const std::vector<Vertex>& vertices) const
{
	GLfloat max_x, max_y, max_z;
	GLfloat min_x = max_x = vertices[0].Position.x;
	GLfloat min_y = max_y = vertices[0].Position.y;
	GLfloat min_z = max_z = vertices[0].Position.z;
	for (int i = 1; i < vertices.size(); i++) {
		if (vertices[i].Position.x < min_x) min_x = vertices[i].Position.x;
		if (vertices[i].Position.x > max_x) max_x = vertices[i].Position.x;
		if (vertices[i].Position.y < min_y) min_y = vertices[i].Position.y;
		if (vertices[i].Position.y > max_y) max_y = vertices[i].Position.y;
		if (vertices[i].Position.z < min_z) min_z = vertices[i].Position.z;
		if (vertices[i].Position.z > max_z) max_z = vertices[i].Position.z;
	}
	const glm::vec3 scale = model->getScale();
	const glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z) * scale;
	glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2) * scale;

	center += model->getPosition();

	return std::array<glm::vec3, 2> { {size, center} };
}

void SkeletalHitbox::generateBonesHitboxes()
{
	//TODO: fix for dynamic objects: rigidbody are linked to the model
	std::map<std::string, int> bones = reinterpret_cast<SkeletalModelData*>(model->getModelData())->hitboxesBones;
	unsigned int index = 0;
	for (auto const& [boneName, boneId] : bones) {
		//Get all vertices affected by the bone
		std::vector<Vertex> v;
		for (Mesh* m : model->getModelData()->meshes) {
			for (Vertex vertex : m->vertices) {
				for (int i = 0; i < 4; i++) {
					if (vertex.m_BoneIDs[i] == boneId) {
						v.emplace_back(vertex);
					}
				}
			}
		}

		if (v.empty()) continue;

		const std::array<glm::vec3, 2> centerSize = getMeshCenterAndSize(v);
		const glm::vec3 size = glm::vec3(centerSize[0] * glm::vec3(0.25));
		const glm::vec3 center = centerSize[1];
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(center.x, center.y, center.z));
		btScalar radius, height;

		const std::string s = boneName;
		SkeletalLoader::BoneHierarchy currentBone = *std::find_if(
			SkeletalLoader::bonesHitboxNames.begin(),
			SkeletalLoader::bonesHitboxNames.end(),
			[&](const SkeletalLoader::BoneHierarchy h)
			{
				return h.name == s;
			});

		if (!currentBone.horizontal) {
			height = centerSize[0].y + currentBone.sizeHeight;
			radius = (size.x + size.z) / 4.0f + currentBone.sizeRadius;
		}
		else {
			height = (size.x + size.z) + currentBone.sizeHeight;
			radius = (size.x + size.z) / 2.0f + currentBone.sizeRadius;
		}



		btVector3 startingInertia2(0, 0, 0);
		auto* box = new btCapsuleShape(radius, height);
		box->calculateLocalInertia(model->getWeight(), startingInertia2);

		rigidBodys.emplace_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
			0.0f,
			nullptr,
			box,
			startingInertia2
		)));

		rigidBodys.back()->setCollisionFlags(rigidBodys.back()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		rigidBodys.back()->setCenterOfMassTransform(transform);
		//TODO: find best way
		//rigidBodys.back()->setActivationState(DISABLE_DEACTIVATION);

		model->setHitboxToBone(boneName, rigidBodys.back());
		index++;
	}
}

void SkeletalHitbox::generateGlobalAABB()
{
	const std::array<glm::vec3, 2> dataSize = getBiggestHitBox();
	//TODO: rename size by extents
	const glm::vec3 size = glm::vec3(dataSize[0] * glm::vec3(0.5));
	const glm::vec3 center = dataSize[1];
	model->setCenter(center);
	model->setSize(size);

	btVector3 startingInertia(0, 0, 0);
	boxCollisionShape = new btBoxShape(btVector3(size.x, size.y, size.z));
	boxCollisionShape->calculateLocalInertia(model->getWeight(), startingInertia);
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(center.x, center.y, center.z));

	rigidBodys.emplace_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
		model->getWeight(),
		nullptr,
		boxCollisionShape,
		startingInertia
	)));

	//TODO: add isStatic or isKinematic and set the flag
	if (model->getWeight() == 0.0f) {
		rigidBodys.back()->setCollisionFlags(rigidBodys.back()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}
	rigidBodys.back()->setMotionState(new MyMotionState(model, transform));
	rigidBodys.back()->setCenterOfMassTransform(transform);
	//TODO: find best way
	rigidBodys.back()->setActivationState(DISABLE_DEACTIVATION);

}

void SkeletalHitbox::setRotationAroundCenter(const float angle)
{
	centerRotation = glm::radians(angle);
	btQuaternion quat;
	quat.setEuler(centerRotation, 0, 0);
	quat.setRotation(btVector3(0, 1, 0), centerRotation);
	for (auto* rigidBody : rigidBodys) {
		rigidBody->getWorldTransform().setRotation(quat);
	}
}

//TODO: is setPosition mandatory ?
void SkeletalHitbox::setWorldTransform(const glm::vec3& position, const glm::quat& rot)
{
	model->setWorldTransform(position, rot, centerRotation);
}


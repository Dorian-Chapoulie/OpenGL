#include "TriangleHitbox.h"
#include "AABBHitbox.h"
#include <glad/glad.h>
#include "MyMotionState.h"

//TODO: FOR ALL HITBOXES (AABB AND MULTIPLE): refactor as compound for non animated models
TriangleHitbox::~TriangleHitbox()
{
	delete boxCollisionShape;
	rigidBodys.clear();
}

std::vector<btRigidBody*>& TriangleHitbox::generateHitBoxes(Model* model)
{
	this->model = model;
	const std::vector<Mesh*> meshes = model->getModelData()->meshes;
	const glm::vec3 scale = model->getScale();
	for (Mesh* m : meshes) {
		const std::vector<Vertex> vertices = m->vertices;
		const std::vector<unsigned int> indices = m->indices;

		auto meshInterface = new btTriangleMesh();
		for (int i = 0; i < indices.size(); i += 3)
		{
			int index1, index2, index3;
			index1 = indices[i];
			index2 = indices[i + 1];
			index3 = indices[i + 2];

			btVector3 v1(vertices[index1].Position.x * scale.x, vertices[index1].Position.y * scale.y, vertices[index1].Position.z * scale.z);
			btVector3 v2(vertices[index2].Position.x * scale.x, vertices[index2].Position.y * scale.y, vertices[index2].Position.z * scale.z);
			btVector3 v3(vertices[index3].Position.x * scale.x, vertices[index3].Position.y * scale.y, vertices[index3].Position.z * scale.z);

			meshInterface->addTriangle(v3, v2, v1, true);
		}

		btCollisionShape* collisionShapeTerrain = new btBvhTriangleMeshShape(meshInterface, true);
		btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(
			model->getWeight(),
			nullptr,
			collisionShapeTerrain,
			btVector3(0, 0, 0)
		);
		rigidBodys.emplace_back(new btRigidBody(rigidBodyConstructionInfo));

		auto* physicsRigidBody = rigidBodys.back();
		btTransform tr;
		tr.setIdentity();
		tr.setOrigin(btVector3(model->getPosition().x, model->getPosition().y, model->getPosition().z));
		physicsRigidBody->setWorldTransform(tr);
	}
	return rigidBodys;
}

void TriangleHitbox::setRotationAroundCenter(const float angle)
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
void TriangleHitbox::setWorldTransform(const glm::vec3& position, const glm::quat& rot)
{
	model->setWorldTransform(position, rot, centerRotation);
}


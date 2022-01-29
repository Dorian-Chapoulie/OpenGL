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
	for (Mesh* m : meshes) {
		const std::vector<Vertex> vertices = m->getVertices();

		auto* triangleMeshTerrain = new btTriangleMesh();

		for (int i = 0; i < vertices.size() - 4; i += 4)
		{
			//TODO: pretty this 
			const glm::vec3 scale = model->getScale();
			btVector3 vertex1(vertices.at(i).Position.x * scale.x, vertices.at(i).Position.y * scale.y, vertices.at(i).Position.z * scale.z);
			btVector3 vertex2(vertices.at(i + 1).Position.x * scale.x, vertices.at(i + 1).Position.y * scale.y, vertices.at(i + 1).Position.z * scale.z);
			btVector3 vertex3(vertices.at(i + 2).Position.x * scale.x, vertices.at(i + 2).Position.y * scale.y, vertices.at(i + 2).Position.z * scale.z);
			triangleMeshTerrain->addTriangle(vertex1, vertex2, vertex3);
			/*p0 = Point3(-10, -10, 0)
p1 = Point3(-10, 10, 0)
p2 = Point3(10, -10, 0)
p3 = Point3(10, 10, 0)
mesh = BulletTriangleMesh()
mesh.addTriangle(p0, p1, p2)
mesh.addTriangle(p1, p2, p3)*/
		}

		btCollisionShape* collisionShapeTerrain = new btBvhTriangleMeshShape(triangleMeshTerrain, true, true);

		btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(
			0.0f,
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


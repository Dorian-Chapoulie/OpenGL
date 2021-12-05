#include "Model.h"
#include "MyMotionState.h"
#include <glm/gtx/quaternion.hpp>
#include "AssimpHelper.hpp"

Model::Model(const std::string& path, const glm::vec3& position, float weight, bool hasHitbox, bool hasMultipleHitboxes,
	glm::vec3 scale)
{
	this->weight = weight;
	this->position = position;
	this->basePosition = position;
	this->hasHitbox = hasHitbox;
	this->scale = scale;
	//this->modelMatrix = glm::scale(glm::mat4(1.0f), scale);
	setPosition(position);
	this->directory = path;
}

Model::Model(const std::string& path, const glm::vec3& position, bool hasHitbox, bool hasMultipleHitboxes,
	glm::vec3 scale)
{
	this->position = position;
	this->basePosition = position;
	this->hasHitbox = hasHitbox;
	this->scale = scale;
	//this->modelMatrix = glm::scale(glm::mat4(1.0f), scale);
	setPosition(position);
	loadModel(path);
	if (hasHitbox) setupHitbox();
	else if (hasMultipleHitboxes) setupHitboxes();
}

void Model::init(const std::string& path, bool hasMultipleHitboxes)
{
	if (hasHitbox) setupHitbox();
	else if (hasMultipleHitboxes) setupHitboxes();

	/*for (auto* m : meshes)
	{
		m->cleanUp();
	}*/
	//std::cout << "model : " << path << " has: " << boxCollisionShapes.size() << " hitboxex " << std::endl;
}

Model::~Model()
{
}

void Model::loadModel(const std::string& path)
{
	this->modelData = ModelLoader::getInstance()->loadModel(path, ModelLoader::DEFAULT);
}

const glm::vec3 Model::getBasePosition() const {
	return basePosition;
}

const glm::vec3 Model::getSize() const {
	//TODO: fix this
	if (sizes.size() > 0) {
		return sizes[0];
	}
	return glm::vec3(0.0f);
}

const glm::vec3 Model::getCenter() const {
	//TODO: fix this
	if (centers.size() > 0) {
		return centers[0];
	}
	return glm::vec3(0.0f);
}

void Model::draw(Shader& shader)
{
	shader.setMatrix("model", modelMatrix);
	for (Mesh* m : modelData->meshes) {
		m->draw(shader);
	}
}

glm::vec3 Model::getPosition() {
	return position;
}

const float Model::getCenterRotation() const {
	return centerRotation;
}

const glm::mat4 Model::getModelMatrix() const
{
	return modelMatrix;
}

void Model::setPosition(const glm::vec3& position)
{
	this->position = position;
	this->modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale);
}

void Model::setRotationAroundCenter(const float angle)
{
	this->centerRotation = glm::radians(angle);
	btQuaternion quat;
	quat.setEuler(centerRotation, 0, 0);
	quat.setRotation(btVector3(0, 1, 0), centerRotation);
	for (auto& rigidBody : rigidBodys) {
		rigidBody->getWorldTransform().setRotation(quat);
	}
}

void Model::setWorldTransform(const glm::vec3& position, const glm::quat& rot)
{
	//glm::quat r = glm::quat(glm::vec3(0, centerRotation, 0));
	setPosition(position);
	modelMatrix *= glm::rotate(glm::mat4(1.0f), centerRotation, glm::vec3(0.f, 1.f, 0.f));
}

std::vector<btRigidBody*> Model::getRigidBodys() const
{
	return rigidBodys;
}

void Model::SetVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
	{
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_WEIGHTS; ++i)
	{
		if (vertex.m_BoneIDs[i] < 0)
		{
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			newBoneInfo.offset = AssimpHelper::ConvertMatrixToGLMFormat(
				mesh->mBones[boneIndex]->mOffsetMatrix);
			m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = m_BoneCounter;
			m_BoneCounter++;
		}
		else
		{
			boneID = m_BoneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

void Model::setupHitboxes()
{
	for (Mesh* mesh : modelData->meshes) {
		const std::vector<glm::vec3> dataSize = getMeshCenterAndSize(mesh->getVertices());
		sizes.emplace_back(glm::vec3(dataSize[0] * glm::vec3(0.5)));
		centers.emplace_back(dataSize[1]);

		const glm::vec3 size = sizes.back();
		const glm::vec3 center = centers.back();

		boxCollisionShapes.emplace_back(new btBoxShape(btVector3(size.x, size.y, size.z)));
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(center.x, center.y, center.z));

		rigidBodys.emplace_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
			weight,
			nullptr,
			boxCollisionShapes.back()
		)));

		rigidBodys.back()->setMotionState(new MyMotionState(this, transform));
		rigidBodys.back()->setCenterOfMassTransform(transform);
		//TODO: find best way
		rigidBodys.back()->setActivationState(DISABLE_DEACTIVATION);
	}
}

//TODO: getBiggest should return array
void Model::setupHitbox()
{
	const std::vector<glm::vec3> dataSize = getBiggestHitBox();
	sizes.emplace_back(glm::vec3(dataSize[0] * glm::vec3(0.5)));
	centers.emplace_back(dataSize[1]);

	const glm::vec3 size = sizes.back();
	const glm::vec3 center = centers.back();

	boxCollisionShapes.emplace_back(new btBoxShape(btVector3(size.x, size.y, size.z)));
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(center.x, center.y, center.z));

	rigidBodys.emplace_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
		weight,
		nullptr,
		boxCollisionShapes.back()
	)));

	rigidBodys.back()->setMotionState(new MyMotionState(this, transform));
	rigidBodys.back()->setCenterOfMassTransform(transform);
	//TODO: find best way
	rigidBodys.back()->setActivationState(DISABLE_DEACTIVATION);

}

std::vector<glm::vec3> Model::getMeshCenterAndSize(const std::vector<Vertex>& vertices) const
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
	glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z) * scale;
	glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2) * scale;

	center += position;

	std::vector<glm::vec3> ret;
	ret.emplace_back(size);
	ret.emplace_back(center);
	return ret;
}

std::vector<glm::vec3> Model::getBiggestHitBox() const
{
	if (modelData->meshes.empty()) return std::vector<glm::vec3> { glm::vec3(0) };
	std::vector<glm::vec3> sizeCenter = getMeshCenterAndSize(modelData->meshes[0]->getVertices());
	glm::vec3 maxSize = glm::vec3(sizeCenter[0]);
	for (Mesh* m : modelData->meshes)
	{
		const std::vector<glm::vec3> temp = getMeshCenterAndSize(m->getVertices());
		const glm::vec3 size = temp[0];
		if (size.x > maxSize.x && size.y > maxSize.y && size.z > maxSize.z)
		{
			maxSize = size;
			sizeCenter = temp;
		}
	}
	return sizeCenter;
}


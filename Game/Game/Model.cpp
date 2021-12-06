#include "Model.h"
#include "AssimpHelper.hpp"

Model::Model(const std::string& path, const glm::vec3& position, float weight,
	glm::vec3 scale)
{
	this->weight = weight;
	this->position = position;
	this->basePosition = position;
	this->scale = scale;
	setPosition(position);
}

Model::Model(const std::string& path, const glm::vec3& position,
	glm::vec3 scale)
{
	this->position = position;
	this->basePosition = position;
	this->scale = scale;
	setPosition(position);
	loadModel(path);
}

glm::vec3 Model::getScale() const
{
	return scale;
}

float Model::getWeight() const
{
	return weight;
}

ModelData* Model::getModelData()
{
	return modelData;
}

IHitBox* Model::getHitBox()
{
	return nullptr;
}

void Model::setWorldTransform(const glm::vec3& position, const glm::quat& rot, float centerRotation)
{
	this->position = position;
	this->modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale);
	modelMatrix *= glm::rotate(glm::mat4(1.0f), centerRotation, glm::vec3(0.f, 1.f, 0.f));
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

void Model::draw(Shader& shader)
{
	shader.setMatrix("model", modelMatrix);
	for (Mesh* m : modelData->meshes) {
		m->draw(shader);
	}
}

void Model::setCenter(glm::vec3 center)
{
	this->center = center;
}

glm::vec3 Model::getCenter() const
{
	return center;
}

glm::vec3 Model::getPosition() {
	return position;
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



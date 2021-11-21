#include "Model.h"
#include "MyMotionState.h"
#include <glm/gtx/quaternion.hpp>
#include "AssimpHelper.hpp"

//TODO: VERIFY EVERY CONSTRUCTORS

Model::Model(const std::string& path, const glm::vec3& position, float weight, bool hasHitbox, bool hasMultipleHitboxes)
{
	this->weight = weight;
	this->position = position;
	this->basePosition = position;
	this->hasHitbox = hasHitbox;
	setPosition(position);
	loadModel(path);
	if (hasHitbox) setupHitbox();
	else if (hasMultipleHitboxes) setupHitboxes();
	std::cout << "model : " << path << " has: " << boxCollisionShapes.size() << " hitboxex " << std::endl;
}

Model::Model(const std::string& path, const glm::vec3& position, float weight, bool hasHitbox, bool hasMultipleHitboxes,
	glm::vec3 scale)
{
	this->weight = weight;
	this->position = position;
	this->basePosition = position;
	this->hasHitbox = hasHitbox;
	this->modelMatrix = glm::scale(this->modelMatrix, scale);
	this->scale = scale;
	setPosition(position);
	loadModel(path);
	if (hasHitbox) setupHitbox();
	else if (hasMultipleHitboxes) setupHitboxes();

	for (auto* m : meshes)
	{
		m->cleanUp();
	}

	std::cout << "model : " << path << " has: " << boxCollisionShapes.size() << " hitboxex " << std::endl;
}

Model::Model(const std::string& path, const glm::vec3& position, float weight, bool hasHitbox)
{
	this->weight = weight;
	this->position = position;
	this->basePosition = position;
	this->hasHitbox = hasHitbox;
	setPosition(position);
	loadModel(path);
	if (hasHitbox) setupHitboxes();
	std::cout << "model : " << path << " has: " << boxCollisionShapes.size() << " hitboxex " << std::endl;
}
//TODO: stipulate that this is a static object
Model::Model(const std::string& path, const glm::vec3& position, bool hasHitbox)
{
	this->position = position;
	this->hasHitbox = hasHitbox;
	this->position = position;
	this->basePosition = position;
	setPosition(position);
	loadModel(path);
	if (hasHitbox) setupHitboxes();
	std::cout << "model : " << path << " has: " << boxCollisionShapes.size() << " hitboxex " << std::endl;
}

Model::Model(const std::string& path, const glm::vec3& position, const glm::vec3& scale, bool hasHitbox)
{
	this->position = position;
	this->hasHitbox = hasHitbox;
	this->position = position;
	this->basePosition = position;
	this->modelMatrix = glm::scale(this->modelMatrix, scale);
	this->scale = scale;
	setPosition(position);
	loadModel(path);
	if (hasHitbox) setupHitboxes();
	std::cout << "model : " << path << " has: " << boxCollisionShapes.size() << " hitboxex " << std::endl;
}

Model::~Model()
{
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
	for (Mesh* m : meshes) {
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
	this->modelMatrix = glm::translate(this->modelMatrix, position);
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
	this->position = position;
	//glm::quat r = glm::quat(glm::vec3(0, centerRotation, 0));
	modelMatrix = glm::translate(glm::mat4(1.0f), position);
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

void Model::loadModel(const std::string& path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//vetices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		SetVertexBoneDataToDefault(vertex);

		if (mesh->mNormals) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		else {
			vertex.Normal = glm::vec3(0.0f);
		}

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		//vertex.Position += position;
		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}


	//TODO create an animated model class
	ExtractBoneWeightForVertices(vertices, mesh, scene);

	return new Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < loadedTextures.size(); j++)
		{
			if (std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory, false);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			loadedTextures.push_back(texture); // add to loaded textures
		}
	}
	return textures;
}

void Model::setupHitboxes()
{
	for (Mesh* mesh : meshes) {
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
		mesh->cleanUp();
	}
}

void Model::setupHitbox()
{
	if (meshes.empty()) return;
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

	for (Mesh* m : meshes)
	{
		m->cleanUp();
	}
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
	if (meshes.empty()) return std::vector<glm::vec3> { glm::vec3(0) };
	std::vector<glm::vec3> sizeCenter = getMeshCenterAndSize(meshes[0]->getVertices());
	glm::vec3 maxSize = glm::vec3(sizeCenter[0]);
	for (Mesh* m : meshes)
	{
		const std::vector<glm::vec3> temp = getMeshCenterAndSize(m->getVertices());
		const glm::vec3 size = temp[0];
		if (size.x > maxSize.x && size.y > maxSize.y && size.z > maxSize.z)
		{
			maxSize = size;
			sizeCenter = temp;
		}
		m->cleanUp();
	}
	return sizeCenter;
}


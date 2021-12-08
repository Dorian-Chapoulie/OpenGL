#include "Model.h"

Model::Model(
	const std::string& path,
	const glm::vec3& position,
	float weight,
	glm::vec3 scale,
	bool isAnimated)
{
	this->directory = path;
	this->weight = weight;
	this->position = position;
	this->basePosition = position;
	this->scale = scale;
	this->isAnimated = isAnimated;
	setPosition(position);
}

Model::Model(const std::string& path, const glm::vec3& position, glm::vec3 scale, bool isAnimated)
{
	this->position = position;
	this->basePosition = position;
	this->scale = scale;
	this->isAnimated = isAnimated;
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

Animation* Model::getAnimation()
{
	return animation;
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
	if (isAnimated)
	{
		this->modelData = ModelLoader::getInstance()->loadModel(path, ModelLoader::SKELETAL);
		this->animation = new Animation(path, reinterpret_cast<SkeletalModelData&>(*modelData), modelMatrix);
	}
	else {
		this->modelData = ModelLoader::getInstance()->loadModel(path, ModelLoader::DEFAULT);
	}
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

glm::mat4& Model::getModelMatrix()
{
	return modelMatrix;
}

void Model::setPosition(const glm::vec3& position)
{
	this->position = position;
	this->modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale);
}

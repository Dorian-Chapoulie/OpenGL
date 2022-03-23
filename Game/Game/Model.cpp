#include "Model.h"

#include <thread>
#include <glm/gtx/euler_angles.hpp>
#include "Animator.h"

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

void Model::setMatrix(glm::mat4 mat)
{
	this->modelMatrix = mat;
}

std::vector<btRigidBody*>& Model::getRigidBodys()
{
	return rigidBodys;
}

void Model::setHitboxToBone(std::string boneName, btRigidBody* hitbox)
{
	bonesHitboxes.insert(std::make_pair(boneName, hitbox));
}

btRigidBody* Model::getHitboxFromBoneName(const std::string& boneName)
{
	return bonesHitboxes[boneName];
}

IHitBox* Model::getHitBox()
{
	return nullptr;
}

//modelMatrix *= glm::rotate(glm::mat4(1.0f), centerRotation, glm::vec3(0.f, 1.f, 0.f));
void Model::setWorldTransform(const glm::vec3& position, const glm::quat& rot, float centerRotation)
{
	this->position = position;
	this->modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale) * glm::toMat4(rot);

	//glm::vec3 euler = glm::eulerAngles(rot);
	//std::cout << euler.x << " " << glm::normalize(glm::vec3(sin(euler.y))).y << " " << euler.z << std::endl;

	//modelMatrix *= glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.f, 0.f, 0.f));

	//modelMatrix *= glm::toMat4(rot);
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

void Model::draw(Shader& shader, Animator& animator, const glm::mat4 viewMatrix)
{
	shader.use();
	shader.setMatrix("view", viewMatrix);
	auto transforms = animator.GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i) {
		shader.setMatrix("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
	}
	this->draw(shader);
}

void Model::setCenter(glm::vec3 center)
{
	this->center = center;
}

glm::vec3 Model::getCenter() const
{
	return center;
}

void Model::setSize(glm::vec3 size)
{
	this->size = size;
}

glm::vec3 Model::getSize() const
{
	return size;
}

glm::vec3 Model::getPosition() {
	return position;
}

glm::vec3 Model::getRotation()
{
	return rotation;
}

float Model::getYRotation() const
{
	return rotationY;
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

void Model::setRotation(const glm::vec3& rotationAxis, float angle)
{
	rotationY = angle;
	this->rotation = rotationAxis;
	this->modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale) * glm::rotate(glm::mat4(1.0f), angle, rotationAxis);
}

#pragma once
#include <string>
#include <glm/vec3.hpp>

#include "Animation.h"
#include "IHitBox.h"
#include "ModelLoader.h"
#include "Shader.h"

class IHitBox;
class Animator;
class Model {
public:
	Model(const std::string& path, const glm::vec3& position, glm::vec3 scale = glm::vec3(1.0f), bool isAnimated = false);
	~Model();

	void draw(Shader& shader);
	void draw(Shader& shader, Animator& animator, const glm::mat4 viewMatrix);

	virtual void setPosition(const glm::vec3& position);
	virtual void setRotation(const glm::vec3& rotationAxis, float angle);
	virtual IHitBox* getHitBox();
	virtual void setWorldTransform(const glm::vec3& position, const glm::quat& rot, float centerRotation);

	void setCenter(glm::vec3 center);
	glm::vec3 getCenter() const;

	void setSize(glm::vec3 size);
	glm::vec3 getSize() const;

	glm::vec3 getPosition();
	glm::vec3 getRotation();
	float getYRotation() const;
	const glm::vec3 getBasePosition() const;
	glm::mat4& getModelMatrix();

	glm::vec3 getScale() const;
	float getWeight() const;
	ModelData* getModelData();
	Animation* getAnimation();

	virtual std::vector<btRigidBody*>& getRigidBodys();
	void setHitboxToBone(std::string boneName, btRigidBody* hitbox);
	btRigidBody* getHitboxFromBoneName(const std::string& boneName);

	std::string directory;
	bool isAnimated = false;
protected:
	Model(const std::string& path, const glm::vec3& position, float weight, glm::vec3 scale, bool isAnimated = false);

	ModelData* modelData;

	glm::vec3 center = glm::vec3(0.0f);
	glm::vec3 size = glm::vec3(0.0f);
	//TODO: loadedModels
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0, 1, 0);

	//TODO: create 3 rotations (x, y, z)
	float rotationY = 0.0f;

	glm::vec3 basePosition = glm::vec3(0.0f);

	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	virtual void loadModel(const std::string& path);
	Animation* animation;

	std::vector<btRigidBody*> rigidBodys;
	std::map<std::string, btRigidBody*> bonesHitboxes;

private:
	float weight = 0.0f;
};



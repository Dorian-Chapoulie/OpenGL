#pragma once
#include <string>
#include <glm/vec3.hpp>

#include "Animation.h"
#include "IHitBox.h"
#include "ModelLoader.h"
#include "Shader.h"

class IHitBox;
class Model {
public:
	Model(const std::string& path, const glm::vec3& position, glm::vec3 scale, bool isAnimated = false);
	~Model();

	void draw(Shader& shader);

	virtual void setPosition(const glm::vec3& position);
	virtual IHitBox* getHitBox();
	virtual void setWorldTransform(const glm::vec3& position, const glm::quat& rot, float centerRotation);

	void setCenter(glm::vec3 center);
	glm::vec3 getCenter() const;

	void setSize(glm::vec3 size);
	glm::vec3 getSize() const;

	glm::vec3 getPosition();
	const glm::vec3 getBasePosition() const;
	glm::mat4& getModelMatrix();

	glm::vec3 getScale() const;
	float getWeight() const;
	ModelData* getModelData();
	Animation* getAnimation();

protected:
	Model(const std::string& path, const glm::vec3& position, float weight, glm::vec3 scale, bool isAnimated = false);

	ModelData* modelData;

	glm::vec3 center = glm::vec3(0.0f);
	glm::vec3 size = glm::vec3(0.0f);
	//TODO: loadedModels
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 basePosition = glm::vec3(0.0f);

	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	virtual void loadModel(const std::string& path);
	bool isAnimated = false;
	Animation* animation;

private:
	float weight = 0.0f;
	std::string directory;
};


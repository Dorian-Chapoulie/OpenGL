#pragma once
#include "Model.h"

class Player
{
public:
	Player(const std::string& bodyModelPath, const glm::vec3& position);
	~Player();

	glm::vec3 getPosition() const;

	virtual void setPosition(const glm::vec3& position);
	void setOrientation(const glm::vec3& orientation);

	void changeModel(const std::string& modelPath);

	virtual void draw(Shader& shader);
	void increaseCameraYOffset();
	void decreaseCameraYOffset();

	Model* getModel() const;

private:
	glm::vec3 position = glm::vec3(1.0f);
	glm::vec3 orientation = glm::vec3(1.0f);
	std::string modelPath;

	void initModel(const std::string& modelPath);


protected:
	Model* model = nullptr;
	float offsetCameraY = 10.0f;
	const float offsetCameraYStep = 1.0f;
};


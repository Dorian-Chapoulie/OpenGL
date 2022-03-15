#pragma once
#include "Light.h"
#include "StaticModel.h"

class Level
{
public:
	Level(Shader& shader);
	~Level();

	virtual void init() = 0;
	virtual std::vector<Light*> loadLights();
	void draw() const;

	Model* getModel();
	std::vector<Light*>& getLights();

protected:
	StaticModel* model = nullptr;
	Shader& shader;
	std::vector<Light*> lights;

	glm::vec3 playerSpawnPoint = glm::vec3(0.0f);
};


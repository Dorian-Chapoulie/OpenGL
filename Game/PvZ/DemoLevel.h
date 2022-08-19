#pragma once
#include "Level.h"

class DemoLevel :
	public Level
{
public:
	DemoLevel(Shader& shader);

	void init() override;
	std::vector<Light*> loadLights() override;

private:
	const glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	const HitBoxFactory::TYPE hitboxType = HitBoxFactory::TRIANGLE;
	const std::string lightFilename = "../../maps/lights.json";
};


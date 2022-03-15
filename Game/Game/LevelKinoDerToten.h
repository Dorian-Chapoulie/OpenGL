#pragma once
#include "Level.h"
class LevelKinoDerToten : public Level
{
public:
	LevelKinoDerToten(Shader& shader);

	void init() override;
	std::vector<Light*> loadLights() override;

private:
	const glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	const HitBoxFactory::TYPE hitboxType = HitBoxFactory::TRIANGLE;
	const std::string lightFilename = "lights.json";
};


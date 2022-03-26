#pragma once
#include "Level.h"
class LevelDungeon : public Level
{
public:
	LevelDungeon(Shader& shader);

	void init() override;
	std::vector<Light*> loadLights() override;

private:
	const glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	const HitBoxFactory::TYPE hitboxType = HitBoxFactory::AABB_MULTIPLE;// HitBoxFactory::TRIANGLE;
	const std::string lightFilename = "../../maps/lights.json";
};


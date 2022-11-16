#pragma once
#include "Level.h"

class DemoLevel :
	public Level
{
public:
	DemoLevel(Shader& shader);

	void init() override;
	void draw() const override;
	std::vector<Light*> loadLights() override;

	void animatePlateforms(double timeStamp);

	std::vector<StaticModel*> plateforms;
	std::vector<Model*> rocks;

private:
	const glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	const HitBoxFactory::TYPE hitboxType = HitBoxFactory::TRIANGLE;
	const std::string lightFilename = "../../maps/lights.json";
};


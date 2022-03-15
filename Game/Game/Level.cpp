#include "Level.h"


Level::Level(Shader& shader) : shader(shader)
{}

Level::~Level()
{
	delete model;
	for (const Light* l : lights) delete l;
}

std::vector<Light*> Level::loadLights()
{
	return {
		new Light(&shader, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
	};
}

void Level::draw() const
{
	model->draw(shader);
}

Model* Level::getModel()
{
	return model;
}

std::vector<Light*>& Level::getLights()
{
	return lights;
}

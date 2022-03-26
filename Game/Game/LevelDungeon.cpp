#include "LevelDungeon.h"

LevelDungeon::LevelDungeon(Shader& shader) : Level(shader)
{
	glm::vec3 scale = glm::vec3(20.0f);
	this->model = new StaticModel("../../maps/dungeon.obj", position, hitboxType, scale);
	//this->navigation = new StaticModel("../../models/kino/kino_navigation.obj", position, HitBoxFactory::AABB_MULTIPLE, scale);
	init();
}

void LevelDungeon::init()
{
	playerSpawnPoint = { -259.383, 40.374, 22.1192 };
	lights = loadLights();
}

std::vector<Light*> LevelDungeon::loadLights()
{
	std::vector<Light*> ret;
	nlohmann::json importJson;
	std::ifstream inputStream(lightFilename);
	inputStream >> importJson;

	for (const auto& light : importJson)
	{
		float p[3];
		float f;
		bool b;
		light.at("position").get_to(p);
		const glm::vec3 position = glm::vec3(p[0], p[1], p[2]);

		light.at("ambiant").get_to(p);
		const glm::vec3 ambiant = glm::vec3(p[0], p[1], p[2]);

		light.at("diffuse").get_to(p);
		const glm::vec3 diffuse = glm::vec3(p[0], p[1], p[2]);

		light.at("specular").get_to(p);
		const glm::vec3 specular = glm::vec3(p[0], p[1], p[2]);

		light.at("direction").get_to(p);
		const glm::vec3 direction = glm::vec3(p[0], p[1], p[2]);

		light.at("linear").get_to(f);
		const float linear = f;

		light.at("quadratic").get_to(f);
		const float quadratic = f;

		light.at("cutOff").get_to(f);
		const float cutOff = f;

		light.at("isDirectional").get_to(b);
		const float isDirectional = b;

		Light* l = new Light(&shader, position, ambiant);
		l->setIsDirectional(isDirectional);
		l->setDiffuse(diffuse);
		l->setSpecular(specular);
		l->setDirection(direction);
		l->setLinear(linear);
		l->setQuadratic(quadratic);
		l->setCutOff(cutOff);
		ret.emplace_back(l);
	}

	return ret;
}

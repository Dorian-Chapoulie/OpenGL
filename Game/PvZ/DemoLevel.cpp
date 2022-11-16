#include "DemoLevel.h"
#include <random>

DemoLevel::DemoLevel(Shader& shader) : Level(shader)
{
	this->model = new StaticModel("../../maps/plateformer/Low Poly Tree House/house.obj", position, hitboxType);
	init();
}

void DemoLevel::init()
{
	playerSpawnPoint = { -40.0f, 251.0f, -17.0f };
	lights = loadLights();

	plateforms.emplace_back(
		new StaticModel("../../maps/plateformer/Floating Island/floating_one.obj", { -40, 0, 200 }, hitboxType, glm::vec3(3.0f, 6.0f, 3.0f))
	);

	rocks.emplace_back(
		new Model("../../maps/plateformer/Floating Island/rock_one.obj", { -100, -80, 250 }, glm::vec3(3.0f))
	);
	rocks.emplace_back(
		new Model("../../maps/plateformer/Floating Island/rock_two.obj", { 50, -60, 180 }, glm::vec3(3.0f))
	);
	rocks.emplace_back(
		new Model("../../maps/plateformer/Floating Island/rock_three.obj", { 50, -30, 300 }, glm::vec3(3.0f))
	);

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(-180, 180);

	for (auto* r : rocks) {
		r->setRotation(glm::vec3(1), dist(mt));
	}

}

void DemoLevel::draw() const
{
	Level::draw();
	for (auto* p : plateforms) {
		p->draw(Level::shader);
	}
	for (auto* r : rocks) {
		r->draw(Level::shader);
	}
}

std::vector<Light*> DemoLevel::loadLights()
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

void DemoLevel::animatePlateforms(double timeStamp)
{
	for (auto* p : plateforms) {
		auto pos = p->getPosition();
		pos.y = sin(timeStamp) * 10;
		p->setPosition(pos);
		p->setRotation({ 0, 1, 0 }, timeStamp * 0.5f);
	}

	for (auto* r : rocks) {
		auto pos = r->getPosition();

		pos.y = sin(timeStamp) * 30 + r->getBasePosition().y;
		r->setPosition(pos);
		r->setRotation({ 1, 1, 1 }, r->getYRotation() + timeStamp * 0.0001f);
	}
}

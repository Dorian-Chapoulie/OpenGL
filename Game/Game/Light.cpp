#include "Light.h"

#include <thread>

Light::Light(Shader* shader, const glm::vec3& position, const glm::vec3& ambient)
{
	this->position = position;
	this->shader = shader;
	this->ambiant = ambient;
	this->id = shader->addLight();
	this->initShader();
}

Light::Light(Shader* shader, const glm::vec3& position, const glm::vec3& direction, const glm::vec3& ambient, float cutOff)
{
	this->shader = shader;
	this->position = position;
	this->direction = new glm::vec3(direction);
	this->ambiant = ambient;
	this->cutOff = new float(cutOff);
	this->isDirectional = true;
	this->id = shader->addLight();
	this->initShader();
}

Light::~Light()
{
	//TODO: remove light from shader
	delete direction;
	delete cutOff;
}

void Light::setPosition(const glm::vec3& position)
{
	this->position = position;
	shader->setVec3("lights[" + std::to_string(id) + "].position", position);
}

void Light::setAmbiant(const glm::vec3& ambiant)
{
	this->ambiant = ambiant;
	shader->setVec3("lights[" + std::to_string(id) + "].ambient", this->ambiant);
}

void Light::setDiffuse(const glm::vec3& diffuse)
{
	this->diffuse = diffuse;
	shader->setVec3("lights[" + std::to_string(id) + "].diffuse", diffuse);
}

void Light::setSpecular(const glm::vec3& specular)
{
	this->specular = specular;
	shader->setVec3("lights[" + std::to_string(id) + "].specular", specular);
}

void Light::setDirection(const glm::vec3& direction)
{
	this->direction = new glm::vec3(direction);
	shader->setVec3("lights[" + std::to_string(id) + "].direction", direction);
}

void Light::setLinear(float linear)
{
	this->linear = linear;
	shader->setValue<float>("lights[" + std::to_string(id) + "].linear", linear);
}

void Light::setQuadratic(float quadratic)
{
	this->quadratic = quadratic;
	shader->setValue<float>("lights[" + std::to_string(id) + "].quadratic", quadratic);
}

void Light::setCutOff(float cutOff)
{
	this->cutOff = new float(cutOff);
	shader->setValue<float>("lights[" + std::to_string(id) + "].cutOff", glm::cos(glm::radians(cutOff)));
}

void Light::initShader() const
{
	shader->setVec3("lights[" + std::to_string(id) + "].position", position);
	shader->setVec3("lights[" + std::to_string(id) + "].ambient", ambiant);
	shader->setVec3("lights[" + std::to_string(id) + "].diffuse", diffuse);// emet
	shader->setVec3("lights[" + std::to_string(id) + "].specular", specular);//reflet

	if (this->isDirectional) {
		shader->setVec3("lights[" + std::to_string(id) + "].direction", *direction);
		shader->setValue<float>("lights[" + std::to_string(id) + "].cutOff", glm::cos(glm::radians(*cutOff))); //radius
		shader->setValue<bool>("lights[" + std::to_string(id) + "].isDirectional", isDirectional);
	} 
	shader->setValue<float>("lights[" + std::to_string(id) + "].linear", linear);
	shader->setValue<float>("lights[" + std::to_string(id) + "].quadratic", quadratic);
	shader->setValue<float>("lights[" + std::to_string(id) + "].constant", constant);
}

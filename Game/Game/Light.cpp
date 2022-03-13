#include "Light.h"

#include <thread>

Light::Light(Shader* shader, const glm::vec3& position, const glm::vec3& ambient)
{
	this->position = position;
	this->shader = shader;
	this->ambiant = ambient;
	this->id = shader->addLight();
	this->initShader();
	this->setupCube();
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
	this->setupCube();
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
	modelMatrix = glm::translate(glm::mat4(1.0f), position);
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

void Light::setIsDirectional(bool isDir)
{
	this->isDirectional = isDir;
	this->direction = new glm::vec3(glm::vec3(0, -1, 0));
	this->cutOff = new float(100.0f);
	initShader();
}

void Light::setConstant(float constant)
{
	this->constant = constant;
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
	}
	shader->setValue<bool>("lights[" + std::to_string(id) + "].isDirectional", isDirectional);
	shader->setValue<float>("lights[" + std::to_string(id) + "].linear", linear);
	shader->setValue<float>("lights[" + std::to_string(id) + "].quadratic", quadratic);
	shader->setValue<float>("lights[" + std::to_string(id) + "].constant", constant);
}

void Light::draw(const glm::mat4& viewMatrix, const glm::mat4 projection)
{
	debugShader.use();

	debugShader.setMatrix("projection", projection);
	debugShader.setMatrix("view", viewMatrix);
	debugShader.setMatrix("model", modelMatrix);
	debugShader.setVec3("lightcolor", ambiant);

	glBindVertexArray(lightCubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

glm::vec3 Light::getPosition() const
{
	return position;
}

glm::vec3 Light::getAmbiant() const
{
	return ambiant;
}

glm::vec3 Light::getDiffuse() const
{
	return diffuse;
}

glm::vec3 Light::getSpecular() const
{
	return specular;
}

glm::vec3 Light::getDirection() const
{
	if (!direction) return glm::vec3(0.0f, -1.0f, 0.0f);
	return *direction;
}

float Light::getConstant() const
{
	return constant;
}

float Light::getLinear() const
{
	return linear;
}

float Light::getQuadratic() const
{
	return quadratic;
}

float Light::getCutOff() const
{
	if (cutOff == nullptr) return 30.0f;
	return *cutOff;
}

bool Light::getIsDirectional() const
{
	return isDirectional;
}

void Light::setupCube()
{
	modelMatrix = glm::translate(modelMatrix, position);

	glGenVertexArrays(1, &lightCubeVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

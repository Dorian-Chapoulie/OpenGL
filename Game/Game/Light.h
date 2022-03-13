#pragma once
#include <glm/vec3.hpp>

#include "Shader.h"

class Light
{
public:
	explicit Light(Shader* shader, const glm::vec3& position);
	Light(Shader* shader, const glm::vec3& position, const glm::vec3& ambient);

	Light(
		Shader* shader,
		const glm::vec3& position,
		const glm::vec3& direction,
		const glm::vec3& ambient,
		float cutOff
	);
	~Light();

	void setPosition(const glm::vec3& position);
	void setAmbiant(const glm::vec3& ambiant);
	void setDiffuse(const glm::vec3& diffuse);
	void setSpecular(const glm::vec3& specular);
	void setDirection(const glm::vec3& direction);
	void setIsDirectional(bool isDirectional);
	void setConstant(float constant);

	void setLinear(float linear);
	void setQuadratic(float quadratic);
	void setCutOff(float cutOff);
	void initShader() const;

	void draw(const glm::mat4& viewMatrix, const glm::mat4 projection);


	glm::vec3 getPosition() const;
	glm::vec3 getAmbiant() const;
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpecular() const;
	glm::vec3 getDirection() const;
	float getConstant() const;
	float getLinear() const;
	float getQuadratic() const;
	float getCutOff() const;
	bool getIsDirectional() const;

private:
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 ambiant = glm::vec3(1.0f);
	glm::vec3 diffuse = glm::vec3(1.0f);
	glm::vec3 specular = glm::vec3(1.0f);
	glm::vec3* direction = nullptr;

	float constant = 0.5f;
	float linear = 0.37f;
	float quadratic = 0.14f;
	float* cutOff = nullptr;

	bool isDirectional = false;

	Shader* shader = nullptr;
	Shader debugShader = Shader("./light.vert", "./light.frag");

	int id = 0;

	//----------------
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	void setupCube();

	GLuint lightCubeVAO, VBO;
	float vertices[108] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

};


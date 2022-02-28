#pragma once
#include "Model.h"

class InstancedModel
{
public:
	InstancedModel(const std::string& path, int amount);

	void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& viewMatrix);
	void setPosition(int index, glm::vec3 position);
	/*
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(matrix, s, rotation, translation, skew, perspective);
	*/

private:

	void init();

	Model* model;
	std::vector<glm::mat4> modelMatrices;
	int amount = 0;

	unsigned int buffer;
};


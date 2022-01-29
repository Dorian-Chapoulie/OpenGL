#pragma once
#include <vector>
#include <glm/fwd.hpp>

#include "Shader.h"

class OpenGLine
{
public:
	OpenGLine(glm::vec3 start, glm::vec3 end);
	~OpenGLine();

	void setMVP(glm::mat4 mvp);
	void setColor(glm::vec3 color);
	void draw(Shader& shader);

private:
	unsigned int VBO, VAO;
	std::vector<float> vertices;
	glm::vec3 startPoint;
	glm::vec3 endPoint;
	glm::mat4 MVP;
	glm::vec3 lineColor;
};


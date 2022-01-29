#include "OpenGLine.h"

OpenGLine::OpenGLine(glm::vec3 start, glm::vec3 end)
{
	startPoint = start;
	endPoint = end;
	lineColor = glm::vec3(1, 1, 1);

	vertices = {
		 start.x, start.y, start.z,
		 end.x, end.y, end.z,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

OpenGLine::~OpenGLine()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void OpenGLine::setMVP(glm::mat4 mvp)
{
	MVP = mvp;
}

void OpenGLine::setColor(glm::vec3 color)
{
	lineColor = color;
}

void OpenGLine::draw(Shader& shader)
{
	shader.setMatrix("MVP", MVP);
	shader.setVec3("colo", lineColor);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 2);
}

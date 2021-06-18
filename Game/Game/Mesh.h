#pragma once
#include <vector>
#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void draw(Shader& shader) const;

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	void setupMesh();
};


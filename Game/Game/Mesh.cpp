#include "Mesh.h"

Mesh::Mesh(
	std::vector<Vertex> vertices,
	std::vector<unsigned int> indices,
	std::vector<Texture> textures) :
	vertices(std::move(vertices)),
	indices(std::move(indices)),
	textures(std::move(textures))
{
	setupMesh();
}

Mesh::~Mesh()
{
	vertices.clear();
	indices.clear();
	textures.clear();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// bones ids
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
	// bones weights
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

	glBindVertexArray(0);
}
//TODO: MATERIAL HANDLING
void Mesh::draw(Shader& shader) const
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		int number = 0;
		std::string name = textures[i].type;
		if (name == "texture_diffuse") {
			number = diffuseNr++;
			name = "diffuse";
		}
		else if (name == "texture_specular") {
			number = specularNr++;
			name = "specular";
		}

		shader.setValue<int>("material." + name, number);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

std::vector<Vertex> Mesh::getVertices()
{
	return vertices;
}

std::vector<unsigned> Mesh::getIndices()
{
	return indices;
}

//TODO: call this
void Mesh::cleanUp()
{
	this->vertices.clear();
}

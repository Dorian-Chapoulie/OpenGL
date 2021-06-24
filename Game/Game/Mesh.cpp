#include "Mesh.h"

Mesh::Mesh(
	std::vector<Vertex> vertices,
	std::vector<unsigned int> indices, 
	std::vector<Texture> textures) :
		vertices(vertices),
		indices(indices),
		textures(textures) 
{
	setupMesh();
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

    glBindVertexArray(0);
}

void Mesh::draw(Shader& shader) const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
 
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);

       //shader.setValue<int>("material." + name + number, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);
	
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

std::vector<glm::vec3> Mesh::getBoundingBow()
{
    GLfloat
        min_x, max_x,
        min_y, max_y,
        min_z, max_z;
    min_x = max_x = this->vertices[0].Position.x;
    min_y = max_y = this->vertices[0].Position.y;
    min_z = max_z = this->vertices[0].Position.z;
    for (int i = 0; i < this->vertices.size(); i++) {
        if (this->vertices[i].Position.x < min_x) min_x = this->vertices[i].Position.x;
        if (this->vertices[i].Position.x > max_x) max_x = this->vertices[i].Position.x;
        if (this->vertices[i].Position.y < min_y) min_y = this->vertices[i].Position.y;
        if (this->vertices[i].Position.y > max_y) max_y = this->vertices[i].Position.y;
        if (this->vertices[i].Position.z < min_z) min_z = this->vertices[i].Position.z;
        if (this->vertices[i].Position.z > max_z) max_z = this->vertices[i].Position.z;
    }

    std::vector<glm::vec3> ret;
    glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
    glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	
    ret.emplace_back(size);
    ret.emplace_back(center);
    return ret;
}

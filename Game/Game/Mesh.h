#pragma once
#include <reactphysics3d/collision/shapes/BoxShape.h>
#include <vector>
#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices,
		std::vector<unsigned int> indices,
		std::vector<Texture> textures,
		const std::string& name,
		const glm::vec3& position,
		bool hasHitbox = true
	);
	~Mesh();
	
	void draw(Shader& shader) const;
	void setBoundingBoxPosition(glm::vec3 pos);
	
	reactphysics3d::CollisionBody* getBody();

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	std::string name;

	glm::vec3 position;
	reactphysics3d::CollisionBody* body = nullptr;
	reactphysics3d::BoxShape* hitbox = nullptr;
	reactphysics3d::Collider* collider = nullptr;

	void setupMesh();
	void setupHitbox();
	std::vector<glm::vec3> getMeshCenterAndSize();
};


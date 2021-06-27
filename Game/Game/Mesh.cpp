#include "Mesh.h"

#include "PhysicsWorld.h"

Mesh::Mesh(
	std::vector<Vertex> vertices,
	std::vector<unsigned int> indices, 
	std::vector<Texture> textures, const std::string& name, const glm::vec3& position, bool hasHitbox) :
		vertices(vertices),
		indices(indices),
		textures(textures),
		name(name),
		position(position)
{
	setupMesh();
    if (hasHitbox) setupHitbox();
}

Mesh::~Mesh()
{
    //PhysicsWorld::getInstance()->getPhysics()->destroyBoxShape(hitbox);
    //PhysicsWorld::getInstance()->getWorld()->destroyCollisionBody(body);
    //delete collider;
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

void Mesh::setupHitbox()
{
    const std::vector<glm::vec3> dataSize = getMeshCenterAndSize();
    const glm::vec3 size = dataSize[0];
    const glm::vec3 center = dataSize[1];

	if (size.x <= 0.0f || size.y <= 0.0f || size.z <= 0.0f)
	{
        std::cerr << "[MESH] ERROR: " << name << " size <= 0 : " << size.x << " " << size.y << " " << size.z << std::endl;
        return;
	}
	
    const reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
    const reactphysics3d::Transform transform(reactphysics3d::Vector3(center.x / 2, center.y / 2, center.z / 2), orientation);
   
    body = PhysicsWorld::getInstance()->getWorld()->createCollisionBody(transform);
    body->setUserData(&name);
    hitbox = PhysicsWorld::getInstance()->getPhysics()->createBoxShape(reactphysics3d::Vector3(size.x / 2, size.y / 2, size.z / 2));
	collider = body->addCollider(hitbox, transform);
}

std::vector<glm::vec3> Mesh::getMeshCenterAndSize()
{
    GLfloat
        min_x, max_x,
        min_y, max_y,
        min_z, max_z;
    min_x = max_x = vertices[0].Position.x;
    min_y = max_y = vertices[0].Position.y;
    min_z = max_z = vertices[0].Position.z;
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i].Position.x < min_x) min_x = vertices[i].Position.x;
        if (vertices[i].Position.x > max_x) max_x = vertices[i].Position.x;
        if (vertices[i].Position.y < min_y) min_y = vertices[i].Position.y;
        if (vertices[i].Position.y > max_y) max_y = vertices[i].Position.y;
        if (vertices[i].Position.z < min_z) min_z = vertices[i].Position.z;
        if (vertices[i].Position.z > max_z) max_z = vertices[i].Position.z;
    }
    glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
    glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);

    center += position * glm::vec3(2.0f);

    std::vector<glm::vec3> ret;
    ret.emplace_back(size);
    ret.emplace_back(center);
    return ret;
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

void Mesh::setBoundingBoxPosition(glm::vec3 pos)
{
	//recalculate vertices + pos in another foo
    return;
    if (this->body == nullptr) return;
    /*this->position = pos;
	const std::vector<glm::vec3> dataSize = test(vertices, pos);
    const glm::vec3 center = dataSize[1];
    const reactphysics3d::Transform tmp(reactphysics3d::Vector3(center.x / 2, center.y / 2, center.z / 2), reactphysics3d::Quaternion::identity());
    body->setTransform(tmp);*/
}

reactphysics3d::CollisionBody* Mesh::getBody()
{
    return body;
}

#include "Model.h"
#include <chrono>

#include "PhysicsWorld.h"


Model::Model(const std::string& path, MODEL_TYPE type, const glm::vec3& position, float scale, bool hasHitbox)
{
    this->type = type;
    this->position = position;
    this->hasHitbox = hasHitbox;
    this->scale = scale;
    this->modelMatrix = glm::mat4(scale);
    this->modelMatrix[3][0] = position.x;
    this->modelMatrix[3][1] = position.y;
    this->modelMatrix[3][2] = position.z;
    loadModel(path);
	setupHitbox();
}

Model::~Model()
{
    for (reactphysics3d::CollisionBody* body : bodys) {
        switch (type)
        {
        case MODEL_TYPE::COLLISION_BODY:
            PhysicsWorld::getInstance()->getWorld()->destroyCollisionBody(body);
            break;
        case MODEL_TYPE::RIGID_BODY:
            PhysicsWorld::getInstance()->getWorld()->destroyRigidBody(reinterpret_cast<reactphysics3d::RigidBody*>(body));
            break;
        }
        //delete body;
    }

    for (reactphysics3d::BoxShape* hitbox : hitboxes) {
        PhysicsWorld::getInstance()->getPhysics()->destroyBoxShape(hitbox);
    }
    //delete collider;
}


void Model::draw(Shader& shader)
{
	for (Mesh* m : meshes) {
        shader.setMatrix("model", modelMatrix);
		m->draw(shader);
        shader.setMatrix("model", glm::mat4(1.0f));
	}
}

void Model::setPosition(const glm::vec3& position)
{
    this->position = position;
    this->modelMatrix[3][0] = this->position.x;
    this->modelMatrix[3][1] = this->position.y;
    this->modelMatrix[3][2] = this->position.z;
    this->modelMatrix = glm::translate(glm::mat4(scale), position);
    //this->modelMatrix *= glm::scale(this->modelMatrix, glm::vec3(scale));
}

void Model::setOriantation(const glm::vec3& oritantion)
{
    this->orientation = orientation;
}

void Model::setMass(float mass)
{
    for (reactphysics3d::CollisionBody* body : bodys) {
        reinterpret_cast<reactphysics3d::RigidBody*>(body)->setMass(mass / bodys.size());
    } 
}

std::vector<reactphysics3d::CollisionBody*> Model::getCollisionBodys() const
{
    return bodys;
}

void Model::loadModel(const std::string& path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    //vetices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        if (mesh->mNormals) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        else {
            vertex.Normal = glm::vec3(0.0f);
        }

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertex.Position += position;
        vertex.Position *= scale;
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

	return new Mesh(vertices, indices, textures);
}
    
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < loadedTextures.size(); j++)
        {
            if (std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(loadedTextures[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory, false);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            loadedTextures.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}

void Model::setupHitbox()
{
    for (const Mesh* mesh : meshes)
    {
        reactphysics3d::CollisionBody* body;
        reactphysics3d::BoxShape* hitbox;
        reactphysics3d::Collider* collider;
    	
        const std::vector<glm::vec3> dataSize = getMeshCenterAndSize(mesh->getVertices());
        const glm::vec3 size = dataSize[0] * scale;
        const glm::vec3 center = dataSize[1] * scale;

        if (size.x <= 0.0f || size.y <= 0.0f || size.z <= 0.0f)
        {
            std::cerr << "[MESH] ERROR: " << "name" << " size <= 0 : " << size.x << " " << size.y << " " << size.z << std::endl;
            return;
        }

        const reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
        const reactphysics3d::Transform transform(reactphysics3d::Vector3(center.x / 2, center.y / 2, center.z / 2), orientation);

        switch (type)
        {
        case MODEL_TYPE::COLLISION_BODY:
            body = PhysicsWorld::getInstance()->getWorld()->createCollisionBody(transform);
            break;
        case MODEL_TYPE::RIGID_BODY:
            body = PhysicsWorld::getInstance()->getWorld()->createRigidBody(transform);
            reinterpret_cast<reactphysics3d::RigidBody*>(body)->setType(reactphysics3d::BodyType::DYNAMIC);
            break;
        case MODEL_TYPE::STATIC:
            body = PhysicsWorld::getInstance()->getWorld()->createRigidBody(transform);
            reinterpret_cast<reactphysics3d::RigidBody*>(body)->setType(reactphysics3d::BodyType::STATIC);
            break;
        default:
            body = PhysicsWorld::getInstance()->getWorld()->createCollisionBody(transform);
            break;
        }
        //TODO: body->setUserData(&name);
        hitbox = PhysicsWorld::getInstance()->getPhysics()->createBoxShape(reactphysics3d::Vector3(size.x / 2, size.y / 2, size.z / 2));
        collider = body->addCollider(hitbox, transform);
        collider->getMaterial().setBounciness(0);

        bodys.emplace_back(body);
        hitboxes.emplace_back(hitbox);
    	colliders.emplace_back(collider);
    }
}

std::vector<glm::vec3> Model::getMeshCenterAndSize(const std::vector<Vertex>& vertices) const
{
	
    GLfloat max_x, max_y, max_z;
    GLfloat min_x = max_x = vertices[0].Position.x;
    GLfloat min_y = max_y = vertices[0].Position.y;
    GLfloat min_z = max_z = vertices[0].Position.z;
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

std::vector<glm::vec3> Model::getBiggestHitBox() const
{
    std::vector<glm::vec3> sizeCenter = getMeshCenterAndSize(meshes[0]->getVertices());
    glm::vec3 maxSize = glm::vec3(sizeCenter[0]);
	for(Mesh* m : meshes)
	{
        const std::vector<glm::vec3> temp = getMeshCenterAndSize(m->getVertices());
        const glm::vec3 size = temp[0];
		if (size.x > maxSize.x && size.y > maxSize.y && size.z > maxSize.z)
		{
            maxSize = size;
            sizeCenter = temp;
		}
	}
    return sizeCenter;
}
#include "Model.h"
#include "MyMotionState.h"

Model::Model(const std::string& path, const glm::vec3& position, float weight, bool hasMultipleHitboxes, bool hasHitbox)
{
    this->weight = weight;
    this->position = position;
    this->basePosition = position;
    this->hasHitbox = hasHitbox;
    setPosition(position);
    loadModel(path);
    if (hasMultipleHitboxes) setupHitboxes();
    else if (hasHitbox) setupHitbox();
}

Model::Model(const std::string& path, const glm::vec3& position, bool hasMultipleHitboxes, bool hasHitbox)
{
    this->position = position;
    this->hasHitbox = hasHitbox;
    this->position = position;
    this->basePosition = position;
    setPosition(position);
    loadModel(path);
    if (hasMultipleHitboxes) setupHitboxes();
    else if (hasHitbox) setupHitbox();
}

Model::~Model()
{
}

const glm::vec3 Model::getBasePosition() const {
    return basePosition;
}

const glm::vec3 Model::getSize() const {
    return size;
}

const glm::vec3 Model::getCenter() const {
    return center;
}

void Model::draw(Shader& shader)
{
	for (Mesh* m : meshes) {
        shader.setMatrix("model", modelMatrix);
		m->draw(shader);
	}
}

glm::vec3 Model::getPosition() {
    return position;
}

void Model::setPosition(const glm::vec3& position)
{
    this->position = position;
    this->modelMatrix = glm::translate(this->modelMatrix, position);
    //printf("%f %f %f\n", position.x, position.y, position.z);
    //this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(scale));
}

void Model::setWorldTransform(const glm::vec3& position)
{
    this->position = position;
    this->modelMatrix = glm::translate(glm::mat4(1.0f), position);
}

btRigidBody* Model::getRigidBody() const
{
    return rigidBody;
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

        //vertex.Position += position;
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

void Model::setupHitboxes()
{
    for (const Mesh* mesh : meshes)
    {

        const std::vector<glm::vec3> dataSize = getMeshCenterAndSize(mesh->getVertices());
        const glm::vec3 size = dataSize[0];
        const glm::vec3 center = dataSize[1];
    	
    }
}

void Model::setupHitbox()
{
	const std::vector<glm::vec3> dataSize = getBiggestHitBox();
    this->size = dataSize[0] * glm::vec3(0.5);
    this->center = dataSize[1];

    boxCollisionShape = new btBoxShape(btVector3(size.x, size.y, size.z));
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(center.x, center.y, center.z));

    rigidBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
        weight,
        nullptr,
        boxCollisionShape
    ));
    btMotionState* motionState = new MyMotionState(this, transform);
    rigidBody->setMotionState(motionState);
    //TODO: find best way
    rigidBody->setActivationState(DISABLE_DEACTIVATION);
    
}

std::vector<glm::vec3> Model::getMeshCenterAndSize(const std::vector<Vertex>& vertices) const
{
	
    GLfloat max_x, max_y, max_z;
    GLfloat min_x = max_x = vertices[0].Position.x;
    GLfloat min_y = max_y = vertices[0].Position.y;
    GLfloat min_z = max_z = vertices[0].Position.z;
    for (int i = 1; i < vertices.size(); i++) {
        if (vertices[i].Position.x < min_x) min_x = vertices[i].Position.x;
        if (vertices[i].Position.x > max_x) max_x = vertices[i].Position.x;
        if (vertices[i].Position.y < min_y) min_y = vertices[i].Position.y;
        if (vertices[i].Position.y > max_y) max_y = vertices[i].Position.y;
        if (vertices[i].Position.z < min_z) min_z = vertices[i].Position.z;
        if (vertices[i].Position.z > max_z) max_z = vertices[i].Position.z;
    }
    glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
    glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);

    center += position;

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

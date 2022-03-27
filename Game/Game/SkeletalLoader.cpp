#include "SkeletalLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "AssimpHelper.hpp"
#include "Mesh.h"

std::map<std::string, SkeletalModelData> SkeletalLoader::cache;

ModelData* SkeletalLoader::loadModel(const std::string& path)
{
	if (cache.contains(path))
	{
		return new SkeletalModelData(cache[path]);
	}

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "error: " << import.GetErrorString() << std::endl;
		throw std::exception(import.GetErrorString());
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

	if (data->hitboxesBones.size() < bonesHitboxNames.size())
	{
		for (const auto& key : bonesHitboxNames)
		{
			if (data->hitboxesBones.find(key.name) == data->hitboxesBones.end())
			{
				//std::cout << "missing bone: " << key.name << std::endl;
			}
		}
	}
	cache[path] = *data;
	return data;
}

void SkeletalLoader::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		data->meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh* SkeletalLoader::processMesh(aiMesh* mesh, const aiScene* scene)
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

		SetVertexBoneDataToDefault(vertex);

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
		}
		else {
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

	ExtractBoneWeightForVertices(vertices, mesh, scene);

	return new Mesh(vertices, indices, textures);
}

std::vector<Texture> SkeletalLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < data->loadedTextures.size(); j++)
		{
			if (std::strcmp(data->loadedTextures[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(data->loadedTextures[j]);
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
			data->loadedTextures.push_back(texture); // add to loaded textures
		}
	}
	return textures;
}

void SkeletalLoader::SetVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
	{
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.0f;
	}
}

void SkeletalLoader::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_WEIGHTS; ++i)
	{
		if (vertex.m_BoneIDs[i] < 0)
		{
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

//TODO: remove unused scene params
void SkeletalLoader::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (data->m_BoneInfoMap.find(boneName) == data->m_BoneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = data->m_BoneCounter;
			newBoneInfo.offset = AssimpHelper::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			data->m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = data->m_BoneCounter;
			data->m_BoneCounter++;
		}
		else
		{
			boneID = data->m_BoneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}

		std::string tmpBoneName = boneName;
		const std::string bannedStr = "mixamorig";
		if (tmpBoneName.find(bannedStr) != std::string::npos) {
			tmpBoneName = tmpBoneName.replace(0, bannedStr.length() + 2, "");
		}

		std::transform(tmpBoneName.begin(), tmpBoneName.end(), tmpBoneName.begin(), ::tolower);
		if (std::find_if(
			bonesHitboxNames.begin(),
			bonesHitboxNames.end(),
			[&](const BoneHierarchy& h) { return h.name == tmpBoneName; }) != bonesHitboxNames.end()
			) {
			data->hitboxesBones.insert(std::pair(tmpBoneName, boneID));
		}
	}
}



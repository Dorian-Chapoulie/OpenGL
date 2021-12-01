#include "SkeletonLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "AssimpHelper.hpp"

LoadData SkeletonLoader::loadModel(const std::string_view& path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(std::string(path), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		const std::string msg = "ERROR ASSIMP: " + std::string(import.GetErrorString());
		throw std::exception(msg.c_str());
	}
	data.directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);

	return data;
}

void SkeletonLoader::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		data.meshes.emplace_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh* SkeletonLoader::processMesh(aiMesh* mesh, const aiScene* scene)
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

		set_vertex_bone_data_to_default(vertex);

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

	extract_bone_weight_for_vertices(vertices, mesh);
	return new Mesh(vertices, indices, textures);
}

std::vector<Texture> SkeletonLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type,
	const std::string& typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < data.loadedTextures.size(); j++)
		{
			if (std::strcmp(data.loadedTextures[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(data.loadedTextures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), std::string(data.directory), false);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			data.loadedTextures.push_back(texture); // add to loaded textures
		}
	}
	return textures;
}


void SkeletonLoader::set_vertex_bone_data_to_default(Vertex& vertex) const
{
	for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
	{
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.0f;
	}
}

void SkeletonLoader::set_vertex_bone_data(Vertex& vertex, int bone_id, float weight) const
{
	for (int i = 0; i < MAX_BONE_WEIGHTS; ++i)
	{
		if (vertex.m_BoneIDs[i] < 0)
		{
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = bone_id;
			break;
		}
	}
}

void SkeletonLoader::extract_bone_weight_for_vertices(const std::vector<Vertex>& vertices, const aiMesh* mesh)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int bone_id = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (data.bone_info_map_.contains(boneName))
		{
			const bone_info newBoneInfo(
				data.bone_counter_,
				AssimpHelper::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix)
			);
			data.bone_info_map_[boneName] = newBoneInfo;
			bone_id = data.bone_counter_;
			data.bone_counter_++;
		}
		else
		{
			bone_id = data.bone_info_map_[boneName].id;
		}
		assert(bone_id != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			set_vertex_bone_data(const_cast<Vertex&>(vertices[vertexId]), bone_id, weight);
		}
	}
}

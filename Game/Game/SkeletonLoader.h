#pragma once
#include <assimp/material.h>
#include <assimp/scene.h>
#include "ILoader.h"

class SkeletonLoader :
	public ILoader
{
public:
	LoadData loadModel(const std::string_view& path) override;

private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

	void extract_bone_weight_for_vertices(const std::vector<Vertex>& vertices, const aiMesh* mesh);
	void set_vertex_bone_data(Vertex& vertex, int bone_id, float weight) const;
	void set_vertex_bone_data_to_default(Vertex& vertex) const;

	const int MAX_BONE_WEIGHTS = 4;
};


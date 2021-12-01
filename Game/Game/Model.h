#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <map>
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "BoneInfo.hpp"
#include <string_view>

#include "LoaderFactory.h"

class Model {
public:
	Model(const std::string_view& path, const glm::vec3& position, const glm::vec3 scale = glm::vec3(1.0f));
	~Model();

	void draw(Shader& shader) const;
	void set_position(const glm::vec3& pos);

	[[nodiscard]] glm::vec3 get_position() const;
	[[nodiscard]] glm::vec3 get_base_position() const;
	[[nodiscard]] glm::mat4* get_model_matrix();
	[[nodiscard]] glm::vec3& get_scale();
	[[nodiscard]] LoadData& get_data();

	//TODO: verifier variblaes à la place de getters à mettre dans protecteds
private:
	//TODO: loadedModels
	std::vector<Texture> loadedTextures;
	std::string directory;

	virtual void loadModel(const std::string_view& path);

protected:
	Model(const glm::vec3& position, const glm::vec3 scale = glm::vec3(1.0f));

	LoadData data;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 base_position = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
};


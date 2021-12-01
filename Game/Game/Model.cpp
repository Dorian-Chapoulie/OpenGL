#include "Model.h"
#include <glm/gtx/quaternion.hpp>
#include "AssimpHelper.hpp"
#include <assimp/postprocess.h>


//TODO: VERIFY EVERY CONSTRUCTORS

Model::Model(const std::string_view& path, const glm::vec3& position, const glm::vec3 scale) :
	position(position),
	scale(scale),
	base_position(position)
{

	this->modelMatrix = glm::scale(this->modelMatrix, scale);

	set_position(position);
	loadModel(path);
}

Model::Model(const glm::vec3& position, const glm::vec3 scale) :
	position(position),
	scale(scale),
	base_position(position)
{
	this->modelMatrix = glm::scale(this->modelMatrix, scale);
	set_position(position);
}

Model::~Model()
{
}


void Model::draw(Shader& shader) const
{
	shader.setMatrix("model", modelMatrix);

	for (int i = 0; i < data.meshes.size(); i++)
	{
		data.meshes[i]->draw(shader);
	}

}

glm::vec3 Model::get_position() const {
	return position;
}

glm::vec3 Model::get_base_position() const
{
	return base_position;
}


glm::mat4* Model::get_model_matrix()
{
	return &modelMatrix;
}

glm::vec3& Model::get_scale()
{
	return scale;
}

LoadData& Model::get_data()
{
	return data;
}

void Model::set_position(const glm::vec3& pos)
{
	this->position = pos;
	this->modelMatrix = glm::translate(this->modelMatrix, position);
}

void Model::loadModel(const std::string_view& path)
{
	data = LoaderFactory::getInstance()->load_model(path, LoaderFactory::DEFAULT);
}




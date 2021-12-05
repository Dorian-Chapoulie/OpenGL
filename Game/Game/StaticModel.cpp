#include "StaticModel.h"

#include "ModelLoader.h"

StaticModel::StaticModel(
	const std::string& path,
	const glm::vec3& position,
	bool hasHitbox,
	bool hasMultipleHitboxes,
	glm::vec3 scale)
	: Model(path, position, 0.0f, hasHitbox, hasMultipleHitboxes, scale)
{
	loadModel(path);
	Model::init(path, false);
}

void StaticModel::loadModel(const std::string& path)
{
	modelData = ModelLoader::getInstance()->loadModel(path, ModelLoader::DEFAULT);
}

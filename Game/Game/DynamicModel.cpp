#include "DynamicModel.h"

#include "ModelLoader.h"

DynamicModel::DynamicModel(const std::string& path, const glm::vec3& position, float weight, bool hasHitbox,
	bool hasMultipleHitboxes, glm::vec3 scale) : Model(path, position, weight, hasHitbox, hasMultipleHitboxes, scale)
{
	loadModel(path);
	Model::init(path, false);
}

void DynamicModel::loadModel(const std::string& path)
{
	modelData = ModelLoader::getInstance()->loadModel(path, ModelLoader::DEFAULT);
}

#include "StaticModel.h"


StaticModel::StaticModel(const std::string_view& path, const glm::vec3& position, glm::vec3 scale)
	: Model(path, position, scale)
{
	//TODO: set soft bodys ?
	for (const auto& mesh : data.meshes) {
		hit_boxes_.emplace_back(new HitBox(this, mesh->getVertices()));
	}
}

StaticModel::~StaticModel() {
	//TODO: clear bodys
}

std::vector<HitBox*>& StaticModel::get_hit_boxes()
{
	return hit_boxes_;
}


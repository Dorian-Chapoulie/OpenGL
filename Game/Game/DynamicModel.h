#pragma once
#include "Model.h"
class DynamicModel :
	public Model
{
public:
	DynamicModel(const std::string& path,
		const glm::vec3& position,
		float weight,
		bool hasHitbox,
		bool hasMultipleHitboxes,
		glm::vec3 scale
	);

private:
	void loadModel(const std::string& path) override;
};


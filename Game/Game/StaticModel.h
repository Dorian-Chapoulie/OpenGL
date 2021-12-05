#pragma once
#include "Model.h"
class StaticModel :
	public Model
{
public:
	StaticModel(const std::string& path,
		const glm::vec3& position,
		bool hasHitbox,
		bool hasMultipleHitboxes,
		glm::vec3 scale
	);
private:
	void loadModel(const std::string& path) override;
};


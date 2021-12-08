#pragma once

#include <glm/glm.hpp>

struct BoneInfo
{
	int id;
	/*offset matrix transforms vertex from model space to bone space*/
	glm::mat4 offset;
};
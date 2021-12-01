#pragma once

#include <glm/glm.hpp>

struct bone_info
{
	int id;
	/*offset matrix transforms vertex from model space to bone space*/
	glm::mat4 offset;
};

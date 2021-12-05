#pragma once
#include <string>
#include "Mesh.h"
#include "ModelLoader.h"

class ILoader
{
public:
	[[nodiscard]] virtual ModelData* loadModel(const std::string& path) = 0;
};


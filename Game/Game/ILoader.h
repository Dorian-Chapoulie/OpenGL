#pragma once
#include <string>
#include "ModelLoader.h"

class ILoader
{
public:
	[[nodiscard]] virtual ModelData* loadModel(const std::string& path) = 0;
};


#pragma once
#include "LoaderFactory.h"
#include "Mesh.h"

class ILoader
{
public:
	virtual ~ILoader() = default;
	virtual LoadData loadModel(const std::string_view& path) = 0;

protected:
	LoadData data;
};


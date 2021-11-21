#pragma once
#include <string>
#include <iostream>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
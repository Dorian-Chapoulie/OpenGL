#pragma once
#include <string>
#include <iostream>
#include <glad/glad.h>
#include "stb_image.h"

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
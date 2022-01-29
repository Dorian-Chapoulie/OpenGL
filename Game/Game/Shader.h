#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:

	Shader(const char* vertexPath, const char* fragmentPath);
	void use();

	template <typename T>
	void setValue(const std::string& name, T value) const {
		if (std::is_same<T, int>::value) {
			glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
		}
		else if (std::is_same<T, bool>::value) {
			glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
		}
		else if (std::is_same<T, float>::value) {
			glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
		}
	}

	void setMatrix(const std::string& name, glm::mat4 mat) const;
	void setVec3(const std::string& name, glm::vec3 vec) const;
	int addLight();
private:
	std::string readFile(const char* fileName) const;
	unsigned int compileShader(const std::string shaderCode, unsigned int type) const;
	void createProgram(unsigned int vertexID, unsigned int fragmentID);
	void init(const std::string vertexCode, const std::string fragmentCode);

	int lightNumber = 0;
	bool isInitialized = false;
	std::string vertexPathName, fragmentPathName;

	static unsigned int currentShaderId;
public:
	unsigned int ID;
};


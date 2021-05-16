#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vShaderCode = readFile(vertexPath);
    std::string fShaderCode = readFile(fragmentPath);
   
    init(vShaderCode, fShaderCode);
}

void Shader::use()
{
    glUseProgram(ID);
}

std::string Shader::readFile(const char* fileName) const
{
    std::string content;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        file.open(fileName);
        std::stringstream vShaderStream;
        vShaderStream << file.rdbuf();
        file.close();
        content = vShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }

    return content;
}

unsigned int Shader::compileShader(const std::string shaderCode, unsigned int type) const
{
    const char* codeChar = shaderCode.c_str();
    int success;
    char infoLog[512];

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &codeChar, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << (type == GL_VERTEX_SHADER ? "GL_VERTEX_SHADER" : "GL_FRAGMENT_SHADER") << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    return shader;
}

void Shader::createProgram(unsigned int vertexID, unsigned int fragmentID)
{  
    this->ID = glCreateProgram();
    glAttachShader(ID, vertexID);
    glAttachShader(ID, fragmentID);
    glLinkProgram(ID);
    
    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

void Shader::init(const std::string vertexCode, const std::string fragmentCode)
{
    unsigned int vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
    unsigned int fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
    createProgram(vertex, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

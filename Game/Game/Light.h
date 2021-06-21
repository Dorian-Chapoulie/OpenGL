#pragma once
#include <glm/vec3.hpp>
#include "Shader.h"

class Light
{
public:
    explicit Light(Shader* shader, const glm::vec3& position);
    Light(Shader* shader, const glm::vec3& position, const glm::vec3& ambient);

    Light(
        Shader* shader,
        const glm::vec3& position,
        const glm::vec3& direction,
        const glm::vec3& ambient,
        float cutOff
    );
    ~Light();

    void setPosition(const glm::vec3& position);
    void setAmbiant(const glm::vec3& ambiant);
    void setDiffuse(const glm::vec3& diffuse);
    void setSpecular(const glm::vec3& specular);
    void setDirection(const glm::vec3& direction);
	
    void setLinear(float linear);
    void setQuadratic(float quadratic);
    void setCutOff(float cutOff);
    void initShader() const;
    
private:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 ambiant = glm::vec3(0.5f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
    glm::vec3* direction = nullptr;

    const float constant = 1.0f;
    float linear = 0.37f;
    float quadratic = 0.14f;
    float* cutOff = nullptr;

    bool isDirectional = false;

    
    Shader* shader = nullptr;

    int id = 0;
};

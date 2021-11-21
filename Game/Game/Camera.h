#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

class Camera
{
public:
	static std::unique_ptr<Camera>& getInstance();

	void setDirection(glm::vec3 direction);
	void setPosition(const glm::vec3 position);
	void setPich(float value);
	void setYaw(float value);

	void increasePosition(glm::vec3 value);
	void increaseYaw(float value);
	void increasePitch(float value);

	float getYaw() const;
	float getPitch() const;
	float getDefaultYaw() const;
	float getDefaultPitch() const;

	glm::vec3 getFrontVector() const;
	glm::vec3 getUpVector() const;
	glm::mat4 getViewMatrix() const;
	glm::vec3 getPosition() const;

private:

	Camera();
	static std::unique_ptr<Camera> instance;

	float yaw = 89.0f, defaultYaw = 89.0f;
	float pitch = 0.0f, defaultPitch = 0.0f;

	glm::vec3 cameraPos = glm::vec3(1.0f);
	glm::vec3 cameraFront = glm::vec3(
		cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
		sin(glm::radians(pitch)),
		sin(glm::radians(yaw)) * cos(glm::radians(pitch))
	);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

};


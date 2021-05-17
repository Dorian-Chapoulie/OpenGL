#include "Camera.h"

std::unique_ptr<Camera> Camera::instance = nullptr;

std::unique_ptr<Camera>& Camera::getInstance()
{
	if (instance == nullptr) instance = std::unique_ptr<Camera>(new Camera());
	return instance;
}

void Camera::setDirection(glm::vec3 direction)
{
	cameraFront = glm::normalize(direction);
}

void Camera::increasePosition(glm::vec3 value)
{
	cameraPos += value;
}

void Camera::increaseYaw(float value)
{
	yaw += value;
}

void Camera::increasePitch(float value)
{
	pitch += value;
}

void Camera::setPich(float value)
{
	pitch = value;
}

void Camera::setYaw(float value)
{
	yaw = value;
}

float Camera::getYaw() const
{
	return yaw;
}

float Camera::getPitch() const
{
	return pitch;
}

glm::vec3 Camera::getFrontVector() const
{
	return cameraFront;
}

glm::vec3 Camera::getUpVector() const
{
	return cameraUp;
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

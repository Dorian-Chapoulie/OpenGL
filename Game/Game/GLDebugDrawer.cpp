#include "GLDebugDrawer.h"

#include <iostream>

#include "Camera.h"
#include "OpenGLine.h"

GLDebugDrawer::GLDebugDrawer()
{
}

GLDebugDrawer::~GLDebugDrawer()
{
}

void GLDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor,
	const btVector3& toColor)
{

}

void GLDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	glm::mat4 projection = glm::perspective(glm::radians(70.0f), static_cast<float>(800 / 600), 0.1f, 100.0f);
	OpenGLine l(glm::vec3(from.getX(), from.getY(), from.getZ()), glm::vec3(to.getX(), to.getY(), to.getZ()));
	l.setColor(glm::vec3(1.0f, 0.0f, 0.0f));
	l.setMVP(projection * Camera::getInstance()->getViewMatrix());
	l.draw();
}

void GLDebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color)
{

}

void GLDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color,
	btScalar alpha)
{

}

void GLDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance,
	int lifeTime, const btVector3& color)
{
}

void GLDebugDrawer::reportErrorWarning(const char* warningString)
{
}

void GLDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
}

void GLDebugDrawer::setDebugMode(int debugMode)
{
}

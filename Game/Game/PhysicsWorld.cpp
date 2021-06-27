#include "PhysicsWorld.h"
#include <vector>
#include "Camera.h"
#include "OpenGLine.h"

PhysicsWorld* PhysicsWorld::instance = nullptr;
PhysicsWorld* PhysicsWorld::getInstance()
{
	if (instance == nullptr)
	{
		instance = new PhysicsWorld();
	}
	return instance;
}

PhysicsWorld::PhysicsWorld()
{
	init();
}

PhysicsWorld::~PhysicsWorld()
{
	physicsCommon.destroyPhysicsWorld(world);
	delete debugRenderer;
	delete projection;
}

reactphysics3d::PhysicsWorld* PhysicsWorld::getWorld()
{
	return world;
}

reactphysics3d::PhysicsCommon *PhysicsWorld::getPhysics()
{
	return &physicsCommon;
}

reactphysics3d::DebugRenderer* PhysicsWorld::getDebugRenderer()
{
	return debugRenderer;
}

void PhysicsWorld::setProjection(glm::mat4* projection)
{
	this->projection = projection;
}

void PhysicsWorld::drawHitBoxes() const
{
	const glm::mat4 view = Camera::getInstance()->getViewMatrix();
	for (int i = 0; i < debugRenderer->getNbLines(); i++)
	{
		reactphysics3d::DebugRenderer::DebugLine debugLine = debugRenderer->getLinesArray()[i];
		OpenGLine line(
			glm::vec3(debugLine.point1.x, debugLine.point1.y, debugLine.point1.z),
			glm::vec3(debugLine.point2.x, debugLine.point2.y, debugLine.point2.z)
		);
		line.setMVP(*(this->projection) * view);
		line.draw();
	}
}

void PhysicsWorld::init()
{
	world = physicsCommon.createPhysicsWorld();
}

void PhysicsWorld::setDebugEnabled(bool isEnabled)
{
	world->setIsDebugRenderingEnabled(true);
	debugRenderer = &world->getDebugRenderer();
	debugRenderer->setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
	debugRenderer->setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
	debugRenderer->setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
	debugRenderer->setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
	debugRenderer->setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);

}

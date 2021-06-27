#pragma once
#include <glm/fwd.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <reactphysics3d/engine/PhysicsCommon.h>

class PhysicsWorld
{
public:
	static PhysicsWorld* getInstance();
	~PhysicsWorld();

	reactphysics3d::PhysicsWorld* getWorld();
	reactphysics3d::PhysicsCommon* getPhysics();
	reactphysics3d::DebugRenderer* getDebugRenderer();

	void setProjection(glm::mat4* projection);

	void drawHitBoxes() const;
	void setDebugEnabled(bool isEnabled);

private:
	PhysicsWorld();
	
	static PhysicsWorld* instance;
	reactphysics3d::PhysicsCommon physicsCommon;
	reactphysics3d::PhysicsWorld* world = nullptr;
	reactphysics3d::DebugRenderer* debugRenderer = nullptr;

	glm::mat4* projection = nullptr;
	
	void init();
};


#pragma once
#include <functional>

#include "Entity.h"
class Trigger :
	public Entity
{
public:
	Trigger();
	Trigger(std::function<void(void*)> foo, glm::vec3 position, float scale = 10.0f);

	void onInit(btDiscreteDynamicsWorld* world) override;
	void onCollide(Entity* other) override;
	void onUpdate(double timeStamp, btDiscreteDynamicsWorld* world) override;

private:
	std::function<void(void*)> onCollideWithPlayerCallback;
	const std::string filePath = "../../models/cube/cube.obj";
};


#pragma once
#include "BasicBullet.h"

class PlayerBullet : public BasicBullet
{
public:
	PlayerBullet(const glm::vec3& position, const glm::vec3& direction)
		: BasicBullet(position, direction)
	{
		this->damage = 50.0f;
	}

	void onInit(btDiscreteDynamicsWorld* world) override;
	void onCollide(Entity* other) override;
	void onUpdate(double timeStamp, btDiscreteDynamicsWorld* world) override;
};


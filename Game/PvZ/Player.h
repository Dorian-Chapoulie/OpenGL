#pragma once
#include "Entity.h"
#include <LocalPlayer.h>

#include "BasicBullet.h"

class PlayerEntity :
	public Entity
{
public:

	PlayerEntity(LocalPlayer* localPlayer);

	void onInit(btDiscreteDynamicsWorld* world) override;
	void onCollide(Entity* other) override;
	void onUpdate(double timeStamp, btDiscreteDynamicsWorld* world) override;

	void shoot(btDiscreteDynamicsWorld* world);
	void removeBullet(int index);

	float health = 100.0f;
	std::vector<BasicBullet*> bullets;
private:
	//float health = 100.0f;
	float timeBuffer = 0.0f;
	float fireRate = 1.0f;
};


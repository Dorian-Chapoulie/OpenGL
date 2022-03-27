#pragma once
#include "Entity.h"
#include <LocalPlayer.h>

#include "PlayerBullet.h"

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

	void playHurtSound() override;
	void playDeathSound() override;

	std::vector<PlayerBullet*> bullets;
private:
	//float health = 100.0f;
	float timeBuffer = 0.0f;
	float fireRate = 1.0f;
};


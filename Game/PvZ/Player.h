#pragma once
#include "Entity.h"
#include <LocalPlayer.h>
class PlayerEntity :
	public Entity
{
public:

	PlayerEntity(LocalPlayer* localPlayer);

	void onInit(btDiscreteDynamicsWorld* world) override;
	void onCollide(Entity* other) override;
	void onUpdate(double timeStamp, btDiscreteDynamicsWorld* world) override;

	float health = 100.0f;
private:
	//float health = 100.0f;
};


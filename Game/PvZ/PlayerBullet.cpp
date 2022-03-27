#include "PlayerBullet.h"

#include "Enemy.h"

void PlayerBullet::onInit(btDiscreteDynamicsWorld* world)
{
	BasicBullet::onInit(world);
}

void PlayerBullet::onCollide(Entity* other)
{
	BasicBullet::onCollide(other);
}

void PlayerBullet::onUpdate(double timeStamp, btDiscreteDynamicsWorld* world)
{
	BasicBullet::onUpdate(timeStamp, world);
}

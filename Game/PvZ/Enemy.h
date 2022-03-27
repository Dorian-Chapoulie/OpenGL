#pragma once
#include "BasicBullet.h"
#include "Entity.h"
class Enemy :
	public Entity
{
public:
	Enemy(const glm::vec3 position);
	void onInit(btDiscreteDynamicsWorld* world) override;
	void onCollide(Entity* other) override;
	void onUpdate(double timeStamp, btDiscreteDynamicsWorld* world) override;

	void shoot(glm::vec3 direction);
	void removeBullet(int index);

	std::vector<BasicBullet*> bullets;
	bool canShoot = true;

private:
	const std::string filePath = "../../models/shooter/shooter.fbx";
	const float scale = 0.05f;
	const float fireRate = 0.5f;
	double timeBuffer = 0.0;
};


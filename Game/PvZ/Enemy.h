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

	std::vector<BasicBullet*> bullets;

private:
	const std::string filePath = "../../models/shooter/shooter.fbx";
	const float scale = 0.01f;
	const int fireRate = 1;
	double timeBuffer = 0.0;
};


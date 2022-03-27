#pragma once
#include "Entity.h"
class BasicBullet :
	public Entity
{
public:
	BasicBullet(glm::vec3 position, glm::vec3 direction);

	void onInit(btDiscreteDynamicsWorld* world) override;
	void onCollide(Entity* other) override;
	void onUpdate(double timeStamp, btDiscreteDynamicsWorld* world) override;

	float getDamage() const;

private:
	const btVector3 customGravity = { 0.0f, 0.0f, 0.0f };
	const std::string filePath = "../../models/shooter_bullet/bullet.fbx";
	glm::vec3 direction;

protected:
	float damage = 10.0f;
};


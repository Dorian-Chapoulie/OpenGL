#include "Enemy.h"

#include <glm/gtx/matrix_decompose.hpp>

#include "EZNgine.h"
#include "StaticModel.h"


Enemy::Enemy(const glm::vec3 position)
{
	weight = 0.0f;
	model = new StaticModel(filePath, position, HitBoxFactory::AABB, glm::vec3(scale));
}

void Enemy::onInit(btDiscreteDynamicsWorld* world)
{
	Entity::onInit(world);
}

void Enemy::onCollide(Entity* other)
{
	std::cout << "Enemy collide" << std::endl;
}

float angleBetween(
	glm::vec3 a,
	glm::vec3 b,
	glm::vec3 origin
) {
	glm::vec3 da = glm::normalize(a - origin);
	glm::vec3 db = glm::normalize(b - origin);
	return glm::acos(glm::dot(da, db));
}
#include <glm/gtx/vector_angle.hpp>


void Enemy::onUpdate(double timeStamp, btDiscreteDynamicsWorld* world)
{
	timeBuffer += timeStamp;

	const glm::vec3 localPlayerPosition = EZNgine::localPlayer->getPosition();
	const glm::mat4 transformation = glm::inverse(glm::lookAt(model->getPosition(), localPlayerPosition, { 0, 1, 0 }));
	glm::vec3 _scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transformation, _scale, rotation, translation, skew, perspective);
	model->setMatrix(transformation * glm::scale(model->getScale()));

	if (timeBuffer >= fireRate)
	{
		glm::vec3 dir = localPlayerPosition - model->getPosition();
		shoot(dir);
		bullets.back()->onInit(world);
		timeBuffer = 0;
	}
}

void Enemy::shoot(glm::vec3 direction)
{
	glm::vec3 pos = model->getPosition();
	pos.y = 15.0f;
	bullets.emplace_back(new BasicBullet(pos, direction));
}

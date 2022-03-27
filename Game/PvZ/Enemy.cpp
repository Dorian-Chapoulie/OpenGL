#include "Enemy.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "EZNgine.h"
#include "Animator.h"
#include "StaticModel.h"
#include "Player.h"
#include "PlayerBullet.h"


Enemy::Enemy(const glm::vec3 position)
{
	weight = 0.0f;
	setModel(new StaticModel(filePath, position, HitBoxFactory::AABB, glm::vec3(scale)));
}

void Enemy::onInit(btDiscreteDynamicsWorld* world)
{
	Entity::onInit(world);
}

void Enemy::onCollide(Entity* other)
{
	const PlayerEntity* playerEntity = dynamic_cast<PlayerEntity*>(other);
	const BasicBullet* bullet = dynamic_cast<BasicBullet*>(other);
	PlayerBullet* playerBullet = dynamic_cast<PlayerBullet*>(other);
	if (playerEntity)
	{
		//std::cout << "Enemy collide with player: " << std::endl;
	}
	else if (playerBullet)
	{
		if (playerBullet->state != ENTITY_STATE::DEAD) {
			playerBullet->state = ENTITY_STATE::DEAD;
			this->life -= bullet->getDamage();

			if (this->life <= 0)
			{
				playDeathSound();
			}
			else
			{
				playHurtSound();
			}

		}
	}
	else if (bullet)
	{
		//std::cout << "Enemy collide with bullet" << std::endl;
	}
}
int i = 0;
void Enemy::onUpdate(double timeStamp, btDiscreteDynamicsWorld* world)
{
	timeBuffer += timeStamp;

	const glm::vec3 localPlayerPosition = EZNgine::localPlayer->getPosition();
	const glm::mat4 transformation = glm::inverse(glm::lookAt(model->getPosition(), localPlayerPosition, { 0, 1, 0 }));
	glm::vec3 s;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transformation, s, rotation, translation, skew, perspective);
	model->setMatrix(
		transformation
		* glm::scale(model->getScale())
		/** glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0))*/
	);

	if (timeBuffer >= fireRate && canShoot)
	{
		constexpr float projectilePower = 50.0f;
		const glm::vec3 dir = glm::normalize(localPlayerPosition - model->getPosition()) * projectilePower;
		shoot(dir);
		bullets.back()->onInit(world);
		timeBuffer = 0;
	}
	Entity::onUpdate(timeStamp, world);
}

void Enemy::shoot(glm::vec3 direction)
{
	glm::vec3 pos = model->getPosition();
	pos.y += 2.0f;
	bullets.emplace_back(new BasicBullet(pos, direction));
}

void Enemy::removeBullet(int index)
{
	bullets.erase(bullets.begin() + index);
}

void Enemy::playHurtSound()
{
	srand(time(NULL));
	int random = rand() % 3;
	irrklang::vec3df position(model->getPosition().x, model->getPosition().y, model->getPosition().z);
	EZNgine::soundEngine->play3D(std::string("../../audio/hit/" + std::to_string(random) + ".wav").c_str(), position);
}

void Enemy::playDeathSound()
{
	srand(time(NULL));
	int random = rand() % 3;
	irrklang::vec3df position(model->getPosition().x, model->getPosition().y, model->getPosition().z);
	EZNgine::soundEngine->play3D(std::string("../../audio/death/" + std::to_string(random) + ".wav").c_str(), position);
}

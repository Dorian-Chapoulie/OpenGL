#include "Player.h"

#include <glm/gtx/matrix_decompose.hpp>

#include "Enemy.h"
#include "EZNgine.h"
#include "Trigger.h"

PlayerEntity::PlayerEntity(LocalPlayer* localPlayer)
{
	setModel(localPlayer->getModel());
}

void PlayerEntity::onInit(btDiscreteDynamicsWorld* world)
{
	Entity::onInit(world);
}

void PlayerEntity::onCollide(Entity* other)
{
	BasicBullet* bullet = dynamic_cast<BasicBullet*>(other);
	const Enemy* enemy = dynamic_cast<Enemy*>(other);
	Trigger* trigger = dynamic_cast<Trigger*>(other);
	if (bullet)
	{
		bullet->state = ENTITY_STATE::DEAD;
		applyDamage(bullet->getDamage());
		if (this->life <= 0)
		{
			playDeathSound();
		}
		else {
			playHurtSound();
		}
	}
	else if (enemy)
	{
		std::cout << "Player collide with enemy" << std::endl;
	}
	else if (trigger)
	{
		trigger->onCollide(this);
	}
}

void PlayerEntity::onUpdate(double timeStamp, btDiscreteDynamicsWorld* world)
{
	timeBuffer += timeStamp;
	Entity::onUpdate(timeStamp, world);
}

void PlayerEntity::shoot(btDiscreteDynamicsWorld* world)
{
	if (timeBuffer < fireRate) return;

	const glm::vec3 cameraPos = EZNgine::localPlayer->getCamera()->getPosition();

	constexpr  float power = -70.f;
	const glm::vec3 forward = glm::mat3(glm::inverse(EZNgine::localPlayer->getCamera()->getViewMatrix())) * glm::vec3(0, 0, 1);
	glm::vec3 origin = cameraPos + forward;
	const glm::vec3 vel = forward * power;

	srand(time(NULL));
	int random = rand() % 2;

	irrklang::vec3df position(model->getPosition().x, model->getPosition().y, model->getPosition().z);
	if (random == 2)
	{
		EZNgine::soundEngine->play3D(std::string("../../audio/throw/" + std::to_string(random) + ".mp3").c_str(), position);
	}
	else
	{
		EZNgine::soundEngine->play3D(std::string("../../audio/throw/" + std::to_string(random) + ".wav").c_str(), position);
	}

	origin.y += 5.0f;
	bullets.emplace_back(new PlayerBullet(origin, vel));
	bullets.back()->onInit(world);
	timeBuffer = 0;
}

void PlayerEntity::removeBullet(int index)
{
	bullets.erase(bullets.begin() + index);
}

void PlayerEntity::playHurtSound()
{
	srand(time(NULL));
	int random = rand() % 3;
	irrklang::vec3df position(model->getPosition().x, model->getPosition().y, model->getPosition().z);
	EZNgine::soundEngine->play3D(std::string("../../audio/hit/" + std::to_string(random) + ".wav").c_str(), position);
}

void PlayerEntity::playDeathSound()
{
	srand(time(NULL));
	int random = rand() % 3;
	irrklang::vec3df position(model->getPosition().x, model->getPosition().y, model->getPosition().z);
	EZNgine::soundEngine->play3D(std::string("../../audio/death/" + std::to_string(random) + ".wav").c_str(), position);
}

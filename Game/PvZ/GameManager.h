#pragma once
#include "BaseApplication.h"
#include "Enemy.h"
#include "LevelKinoDerToten.h"
#include "EZNgine.h"
#include "Player.h"
#include "Trigger.h"

class GameManager : public BaseApplication
{

public:
	void processInput(void* w) override;
	void loop(Shader& shader, double timeStamp);
	void loopInstancied(Shader& shader, double timeStamp);
	void onInitialized(EZNgine* engine) override;
	void manageEntity(Entity* e, double timeStamp, Shader& shader);

	void checkCollisions();
	void checkLifeTime(Entity* e);
	void deleteEntity(Entity* e);

private:
	bool forward = false, backward = false, left = false, right = false, jump = false;

	LevelKinoDerToten* map;
	std::vector<Enemy*> enemys;
	btDiscreteDynamicsWorld* world;
	PlayerEntity* playerEntity;

	Trigger* triggerTest;
};

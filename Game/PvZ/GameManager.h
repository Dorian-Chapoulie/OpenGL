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

	GameManager(glm::mat4 proj);

	void processInput(void* w) override;
	void loop(Shader& shader, double timeStamp);
	void loopInstancied(Shader& shader, double timeStamp);
	void onInitialized(EZNgine* engine) override;
	void manageEntity(Entity* e, double timeStamp, Shader& shader);

	void checkCollisions();
	void checkLifeTime(Entity* e);
	void deleteEntity(Entity* e);
	void checkPlayerFloorColision(LocalPlayer* lp, double timeStamp);
	void manageLights(Shader& shader);

	bool raycastWorld(btVector3 Start, btVector3 End);


	void imGuiLights(Shader& shader, std::vector<Light*>& lights, LocalPlayer* lp);
	void setWindow(GLFWwindow* window);

private:
	bool forward = false, backward = false, left = false, right = false, jump = false;
	int comboCurrentItem = 0;

	const float GRAVITY_Y = -20.f;

	LevelKinoDerToten* map;
	std::vector<Enemy*> enemys;
	btDiscreteDynamicsWorld* world;
	PlayerEntity* playerEntity;

	Trigger* triggerTest;

	glm::mat4 projection;
	GLFWwindow* window = nullptr;
};

#pragma once
#include "BaseApplication.h"

class GameManager : public BaseApplication
{
public:
	GameManager(glm::mat4 projection);

	void loop(Shader& shader, double timeStamp) override;
	void loopInstancied(Shader& shader, double timeStamp) override;
	void loopAnimated(Shader& shader, double timeStamp) override;
	void processInput(void* window) override;
	void onInitialized(EZNgine* engine) override;

private:
	bool forward = false, backward = false, left = false, right = false, jump = false;
	glm::mat4 projection;
};

/*	void manageEntity(Entity* e, double timeStamp, Shader& shader, bool shouldDraw = false);

	void checkCollisions();
	void checkLifeTime(Entity* e);
	void deleteEntity(Entity* e);
	void checkPlayerFloorColision(LocalPlayer* lp, double timeStamp);
	void manageLights(Shader& shader);
	void onLocalPlayerDie();
	void initTrigers();

	bool raycastWorld(btVector3 Start, btVector3 End);


	void imGuiLights(Shader& shader, std::vector<Light*>& lights, LocalPlayer* lp);
	void setWindow(GLFWwindow* window);

private:
	bool forward = false, backward = false, left = false, right = false, jump = false;
	int comboCurrentItem = 0;
	bool isPlaying = true;
	bool canPlayAnimations = false;

	const float GRAVITY_Y = -20.f;

	LevelDungeon* map;
	std::vector<Enemy*> enemys;
	btDiscreteDynamicsWorld* world;
	PlayerEntity* playerEntity;

	Trigger* triggerTest;

	glm::mat4 projection;
	GLFWwindow* window = nullptr;

	StaticModel* door = nullptr;
	const std::string doorPath = "../../models/door/door.obj";

	Enemy* boss = nullptr;

	std::vector<Actor*> actors;
*/
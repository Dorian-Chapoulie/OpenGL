#pragma once
#include "BaseApplication.h"
#include "DemoLevel.h"
#include "Level.h"
#include "Model.h"

class GameManager : public BaseApplication
{
public:
	GameManager(glm::mat4 projection);

	void loop(Shader& shader, double frameTime) override;
	void loopInstancied(Shader& shader, double frameTime) override;
	void loopAnimated(Shader& shader, double frameTime) override;
	void processInput(void* window) override;
	void onInitialized(EZNgine* engine) override;

private:
	const float GRAVITY_Y = -20.f;

	bool forward = false, backward = false, left = false, right = false, jump = false;
	glm::mat4 projection;

	double currentTimestamp;

	Animator* animator;

	DemoLevel* demoLevel;

	btDiscreteDynamicsWorld* dynamicsWorld;

	void checkPlayerFloorColision(double timeStamp);
	bool raycastWorld(btVector3 Start, btVector3 End);
};

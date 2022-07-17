#pragma once
#include "BaseApplication.h"
#include "DemoLevel.h"
#include "Level.h"
#include "Model.h"

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

	double currentTimestamp;

	Model* animatedModel;
	Model* cube;
	Animator* animator;

	DemoLevel* demoLevel;
};

#pragma once
#include "Shader.h"

class EZNgine;
class BaseApplication
{
public:
	BaseApplication();
	virtual void loop(Shader& shader, double timeStamp) = 0;
	virtual void loopInstancied(Shader& shader, double timeStamp) = 0;
	virtual void processInput(void* window) = 0;
	virtual void onInitialized(EZNgine* engine) = 0;

	bool mouseEnabled = true, firstMouse = true;
};


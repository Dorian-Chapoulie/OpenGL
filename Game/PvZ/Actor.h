#pragma once
#include "Model.h"
#include "Animator.h"

class Actor
{
public:

	Actor(Model* m);

	Model* model;
	Animator* animator;
};


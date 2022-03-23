#pragma once
#include "Model.h"

class Entity
{
public:
	virtual void onInit(btDiscreteDynamicsWorld* world);
	virtual void onCollide(Entity* other);
	virtual void onUpdate(double timeStamp, btDiscreteDynamicsWorld* world);

	Model* model;

protected:
	float life = 100.0f;
	float weight = 1.0f;
};


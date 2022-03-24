#pragma once
#include "Model.h"

class Entity
{
public:

	~Entity();

	virtual void onInit(btDiscreteDynamicsWorld* world);
	virtual void onCollide(Entity* other);
	virtual void onUpdate(double timeStamp, btDiscreteDynamicsWorld* world);

	void setModel(Model* model);
	float getMaxLifeTime() const;
	float getLifeTime() const;

	Model* model;

	enum class ENTITY_STATE
	{
		ALIVE,
		DEAD
	};

	ENTITY_STATE state = ENTITY_STATE::ALIVE;

protected:
	float MAX_LIFETIME = std::numeric_limits<float>::max();
	float lifeTime = 0.0f;
	float life = 100.0f;
	float weight = 1.0f;
};


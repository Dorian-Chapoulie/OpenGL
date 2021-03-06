#pragma once
#include "IHitBox.h"

class HitBoxFactory
{
	static HitBoxFactory* instance;

public:
	static HitBoxFactory* getInstance();

	enum TYPE
	{
		AABB,
		AABB_MULTIPLE,
		TRIANGLE,
		SKELETAL,
		NONE,
	};

	IHitBox* getHitBoxes(TYPE type);
};


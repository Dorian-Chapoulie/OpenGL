#include "HitBoxFactory.h"
#include "AABBHitbox.h"
#include "AABBMultipleHitBox.h"
#include "TriangleHitbox.h"

HitBoxFactory* HitBoxFactory::instance = nullptr;

HitBoxFactory* HitBoxFactory::getInstance()
{
	if (instance == nullptr) instance = new HitBoxFactory();
	return instance;
}

IHitBox* HitBoxFactory::getHitBoxes(TYPE type)
{
	switch (type)
	{
	case AABB:
		return new AABBHitbox();
	case AABB_MULTIPLE:
		return new AABBMultipleHitBox();
	case TRIANGLE:
		return new TriangleHitbox();
	default:
	case NONE:
		return nullptr;
	}
}

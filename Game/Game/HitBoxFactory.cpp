#include "HitBoxFactory.h"
#include "AABBHitbox.h"
#include "AABBMultipleHitBox.h"
#include "SkeletalHitbox.h"
#include "TriangleHitbox.h"
#include "CapsuleHitbox.h"

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
	case SKELETAL:
		return new SkeletalHitbox();
	case CAPSULE:
		return new CapsuleHitbox();
	default:
	case NONE:
		return nullptr;
	}
}
